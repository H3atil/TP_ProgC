#include "client.h"
#include "bmp.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define TAILLE_MESSAGE 8192
#define NOMBRE_COULEURS_MAX 30

static int envoyer_tout(int socketfd, const char *donnees, size_t longueur)
{
    size_t envoyes = 0;
    while (envoyes < longueur)
    {
        ssize_t resultat = send(socketfd, donnees + envoyes,
                                longueur - envoyes, MSG_NOSIGNAL);
        if (resultat < 0 && errno == EINTR)
            continue;
        if (resultat <= 0)
        {
            if (resultat < 0)
                perror("send");
            return -1;
        }
        envoyes += (size_t)resultat;
    }
    return 0;
}

static int envoyer_ligne(int socketfd, const char *message)
{
    size_t longueur = strlen(message);
    if (envoyer_tout(socketfd, message, longueur) != 0)
        return -1;
    return envoyer_tout(socketfd, "\n", 1);
}

static int recevoir_ligne(int socketfd, char *message, size_t capacite)
{
    size_t longueur = 0;
    while (longueur + 1 < capacite)
    {
        char caractere;
        ssize_t resultat = recv(socketfd, &caractere, 1, 0);
        if (resultat < 0 && errno == EINTR)
            continue;
        if (resultat < 0)
        {
            perror("recv");
            return -1;
        }
        if (resultat == 0)
            return longueur == 0 ? 0 : -1;
        if (caractere == '\n')
        {
            message[longueur] = '\0';
            return 1;
        }
        message[longueur++] = caractere;
    }
    fprintf(stderr, "Réponse du serveur trop longue.\n");
    return -1;
}

static int ajouter_chaine_json(char *json, size_t capacite, size_t *utilise,
                               const char *chaine)
{
    if (*utilise >= capacite || capacite - *utilise < 2)
        return 0;
    json[(*utilise)++] = '"';

    for (const unsigned char *caractere = (const unsigned char *)chaine;
         *caractere != '\0'; caractere++)
    {
        const char *echappement = NULL;
        switch (*caractere)
        {
            case '"': echappement = "\\\""; break;
            case '\\': echappement = "\\\\"; break;
            case '\b': echappement = "\\b"; break;
            case '\f': echappement = "\\f"; break;
            case '\n': echappement = "\\n"; break;
            case '\r': echappement = "\\r"; break;
            case '\t': echappement = "\\t"; break;
            default: break;
        }

        if (echappement != NULL)
        {
            if (capacite - *utilise < 2)
                return 0;
            json[(*utilise)++] = echappement[0];
            json[(*utilise)++] = echappement[1];
        }
        else if (*caractere < 0x20)
        {
            if (capacite - *utilise < 6)
                return 0;
            int ecrit = snprintf(json + *utilise, capacite - *utilise,
                                 "\\u%04x", (unsigned int)*caractere);
            if (ecrit != 6)
                return 0;
            *utilise += (size_t)ecrit;
        }
        else
        {
            if (*utilise + 1 >= capacite)
                return 0;
            json[(*utilise)++] = (char)*caractere;
        }
    }

    if (*utilise + 1 >= capacite)
        return 0;
    json[(*utilise)++] = '"';
    json[*utilise] = '\0';
    return 1;
}

static int creer_json(char *json, size_t capacite, const char *code,
                      const char *const valeurs[], size_t nombre_valeurs)
{
    size_t utilise = 0;
    int longueur = snprintf(json, capacite, "{\"code\":");
    if (longueur < 0 || (size_t)longueur >= capacite)
        return 0;
    utilise = (size_t)longueur;
    if (!ajouter_chaine_json(json, capacite, &utilise, code))
        return 0;

    const char *milieu = ",\"valeurs\":[";
    size_t longueur_milieu = strlen(milieu);
    if (capacite - utilise <= longueur_milieu)
        return 0;
    memcpy(json + utilise, milieu, longueur_milieu + 1);
    utilise += longueur_milieu;

    for (size_t i = 0; i < nombre_valeurs; i++)
    {
        if (i > 0)
        {
            if (utilise + 1 >= capacite)
                return 0;
            json[utilise++] = ',';
            json[utilise] = '\0';
        }
        if (!ajouter_chaine_json(json, capacite, &utilise, valeurs[i]))
            return 0;
    }

    if (capacite - utilise < 3)
        return 0;
    memcpy(json + utilise, "]}", 3);
    return 1;
}

static int envoyer_requete_json(int socketfd, const char *code,
                                const char *const valeurs[],
                                size_t nombre_valeurs,
                                char *reponse, size_t capacite_reponse)
{
    char requete[TAILLE_MESSAGE];
    if (!creer_json(requete, sizeof(requete), code, valeurs, nombre_valeurs))
    {
        fprintf(stderr, "Requête JSON trop longue.\n");
        return -1;
    }
    if (envoyer_ligne(socketfd, requete) != 0)
        return -1;
    return recevoir_ligne(socketfd, reponse, capacite_reponse) == 1 ? 0 : -1;
}

static int envoyer_couleurs(int socketfd, char *chemin, int nombre)
{
    couleur_compteur *compteur = analyse_bmp_image(chemin);
    if (compteur == NULL)
        return -1;

    size_t nombre_a_envoyer = (size_t)nombre;
    if (nombre_a_envoyer > (size_t)compteur->size)
        nombre_a_envoyer = (size_t)compteur->size;

    char texte_nombre[12];
    char couleurs[NOMBRE_COULEURS_MAX][8];
    const char *valeurs[NOMBRE_COULEURS_MAX + 1];
    (void)snprintf(texte_nombre, sizeof(texte_nombre), "%zu", nombre_a_envoyer);
    valeurs[0] = texte_nombre;

    for (size_t i = 0; i < nombre_a_envoyer; i++)
    {
        unsigned int rouge;
        unsigned int vert;
        unsigned int bleu;
        if (compteur->compte_bit == BITS24)
        {
            couleur24 c = compteur->cc.cc24[i].c;
            rouge = c.rouge;
            vert = c.vert;
            bleu = c.bleu;
        }
        else
        {
            couleur32 c = compteur->cc.cc32[i].c;
            rouge = c.rouge;
            vert = c.vert;
            bleu = c.bleu;
        }
        int longueur = snprintf(couleurs[i], sizeof(couleurs[i]),
                                "#%02x%02x%02x", rouge, vert, bleu);
        if (longueur != 7)
        {
            liberer_couleur_compteur(compteur);
            return -1;
        }
        valeurs[i + 1] = couleurs[i];
    }

    char reponse[1024];
    int statut = envoyer_requete_json(socketfd, "couleurs", valeurs,
                                      nombre_a_envoyer + 1,
                                      reponse, sizeof(reponse));
    liberer_couleur_compteur(compteur);
    if (statut != 0)
        return -1;
    printf("Réponse JSON : %s\n", reponse);
    return 0;
}

int envoie_recois_message(int socketfd)
{
    char message[1024];
    char reponse[TAILLE_MESSAGE];

    printf("Votre message (ou « calcule : op num1 num2 ») : ");
    if (fgets(message, sizeof(message), stdin) == NULL)
        return 1;

    size_t longueur = strlen(message);
    if (longueur > 0 && message[longueur - 1] == '\n')
        message[longueur - 1] = '\0';
    else
    {
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF)
            ;
    }

    if (strncmp(message, "calcule :", 9) == 0)
    {
        char operateur;
        char surplus;
        char operateur_texte[2];
        char num1_texte[32];
        char num2_texte[32];
        int num1;
        int num2;
        if (sscanf(message + 9, " %c %d %d %c",
                   &operateur, &num1, &num2, &surplus) != 3)
        {
            fprintf(stderr, "Format attendu : calcule : <op> <num1> <num2>\n");
            return 0;
        }
        operateur_texte[0] = operateur;
        operateur_texte[1] = '\0';
        (void)snprintf(num1_texte, sizeof(num1_texte), "%d", num1);
        (void)snprintf(num2_texte, sizeof(num2_texte), "%d", num2);
        const char *valeurs[] = {operateur_texte, num1_texte, num2_texte};
        if (envoyer_requete_json(socketfd, "calcule", valeurs, 3,
                                 reponse, sizeof(reponse)) != 0)
            return -1;
        printf("Réponse JSON : %s\n", reponse);
        return 0;
    }

    const char *valeurs[] = {message};
    if (envoyer_requete_json(socketfd, "message", valeurs, 1,
                             reponse, sizeof(reponse)) != 0)
        return -1;
    printf("Réponse JSON : %s\n", reponse);
    return 0;
}

static int connecter_serveur(void)
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        perror("socket");
        return -1;
    }

    struct sockaddr_in adresse;
    memset(&adresse, 0, sizeof(adresse));
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(PORT);
    adresse.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(socketfd, (struct sockaddr *)&adresse, sizeof(adresse)) < 0)
    {
        perror("connection serveur");
        close(socketfd);
        return -1;
    }
    return socketfd;
}

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        fprintf(stderr, "Usage : %s [image.bmp]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int socketfd = connecter_serveur();
    if (socketfd < 0)
        return EXIT_FAILURE;

    int code = EXIT_SUCCESS;
    if (argc == 2)
    {
        int nombre;
        printf("Nombre de couleurs à traiter (1 à 30) : ");
        if (scanf("%d", &nombre) != 1 || nombre < 1
            || nombre > NOMBRE_COULEURS_MAX)
        {
            fprintf(stderr, "Le nombre de couleurs doit être compris entre 1 et 30.\n");
            code = EXIT_FAILURE;
        }
        else if (envoyer_couleurs(socketfd, argv[1], nombre) != 0)
            code = EXIT_FAILURE;
    }
    else if (argc == 1)
    {
        for (;;)
        {
            int statut = envoie_recois_message(socketfd);
            if (statut == 1)
                break;
            if (statut < 0)
            {
                code = EXIT_FAILURE;
                break;
            }
        }
    }
    if (close(socketfd) != 0)
    {
        perror("close");
        code = EXIT_FAILURE;
    }
    return code;
}
