#include "client.h"

#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

static int envoyer_tout(int socketfd, const char *donnees, size_t longueur)
{
    size_t envoyes = 0;
    while (envoyes < longueur)
    {
        ssize_t resultat = send(socketfd, donnees + envoyes, longueur - envoyes, 0);
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

int envoyer_ligne(int socketfd, const char *message)
{
    size_t longueur = strlen(message);
    if (envoyer_tout(socketfd, message, longueur) != 0)
        return -1;
    return envoyer_tout(socketfd, "\n", 1);
}

int recevoir_ligne(int socketfd, char *message, size_t capacite)
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

    fprintf(stderr, "Message reçu trop long.\n");
    return -1;
}

int envoie_operateur_numeros(int socketfd, char operateur,
                             int num1, int num2, int *resultat)
{
    char requete[128];
    char reponse[256];
    int longueur = snprintf(requete, sizeof(requete), "calcule : %c %d %d",
                            operateur, num1, num2);
    if (longueur < 0 || (size_t)longueur >= sizeof(requete)
        || envoyer_ligne(socketfd, requete) != 0)
        return -1;

    int statut = recevoir_ligne(socketfd, reponse, sizeof(reponse));
    if (statut != 1)
        return -1;
    if (sscanf(reponse, "calcule : %d", resultat) != 1)
    {
        fprintf(stderr, "Réponse de calcul invalide : %s\n", reponse);
        return -1;
    }
    return 0;
}

static int calculer_notes_etudiants(int socketfd)
{
    const char *repertoire = access("../etudiant", R_OK) == 0
        ? "../etudiant" : "TP5/etudiant";
    int sommes[5];

    for (int etudiant = 1; etudiant <= 5; etudiant++)
    {
        int somme = 0;
        for (int note = 1; note <= 5; note++)
        {
            char chemin[256];
            int valeur;
            int longueur = snprintf(chemin, sizeof(chemin),
                                    "%s/%d/note%d.txt",
                                    repertoire, etudiant, note);
            if (longueur < 0 || (size_t)longueur >= sizeof(chemin))
            {
                fprintf(stderr, "Chemin de note trop long.\n");
                return -1;
            }

            FILE *fichier = fopen(chemin, "r");
            if (fichier == NULL)
            {
                perror(chemin);
                return -1;
            }
            int lecture_ok = fscanf(fichier, "%d", &valeur) == 1;
            if (fclose(fichier) != 0)
            {
                perror("fclose");
                return -1;
            }
            if (!lecture_ok)
            {
                fprintf(stderr, "Note invalide dans %s.\n", chemin);
                return -1;
            }

            if (note == 1)
                somme = valeur;
            else if (envoie_operateur_numeros(socketfd, '+', somme,
                                              valeur, &somme) != 0)
                return -1;
        }

        sommes[etudiant - 1] = somme;
        int moyenne;
        if (envoie_operateur_numeros(socketfd, '/', somme, 5, &moyenne) != 0)
            return -1;
        printf("Moyenne de l'étudiant %d : %d\n", etudiant, moyenne);
    }

    int somme_classe = sommes[0];
    for (int i = 1; i < 5; i++)
    {
        if (envoie_operateur_numeros(socketfd, '+', somme_classe,
                                     sommes[i], &somme_classe) != 0)
            return -1;
    }

    int moyenne_classe;
    if (envoie_operateur_numeros(socketfd, '/', somme_classe, 25,
                                 &moyenne_classe) != 0)
        return -1;
    printf("Moyenne de la classe : %d\n", moyenne_classe);
    return 0;
}

int envoie_recois_message(int socketfd)
{
    char message[1024];
    char reponse[1024];

    printf("Votre message (ou « etudiants » pour les notes, max 1000 caractères): ");
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

    if (strcmp(message, "etudiants") == 0)
        return calculer_notes_etudiants(socketfd);

    if (strncmp(message, "calcule :", 9) == 0)
    {
        char operateur;
        char surplus;
        int num1;
        int num2;
        if (sscanf(message + 9, " %c %d %d %c",
                   &operateur, &num1, &num2, &surplus) != 3)
        {
            fprintf(stderr, "Format attendu : calcule : <op> <num1> <num2>\n");
            return 0;
        }

        int resultat;
        if (envoie_operateur_numeros(socketfd, operateur, num1, num2,
                                     &resultat) != 0)
            return -1;
        printf("Message reçu: calcule : %d\n", resultat);
        return 0;
    }

    char requete[1050];
    int longueur_requete = snprintf(requete, sizeof(requete),
                                    "message: %s", message);
    if (longueur_requete < 0 || (size_t)longueur_requete >= sizeof(requete)
        || envoyer_ligne(socketfd, requete) != 0)
        return -1;

    int statut = recevoir_ligne(socketfd, reponse, sizeof(reponse));
    if (statut <= 0)
        return -1;
    printf("Message reçu: %s\n", reponse);
    return 0;
}

int main(void)
{
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

    if (connect(socketfd, (struct sockaddr *)&server_addr,
                sizeof(server_addr)) < 0)
    {
        perror("connection serveur");
        close(socketfd);
        return EXIT_FAILURE;
    }

    int code = EXIT_SUCCESS;
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

    if (close(socketfd) != 0)
    {
        perror("close");
        code = EXIT_FAILURE;
    }
    return code;
}
