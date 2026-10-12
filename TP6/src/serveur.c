#include "serveur.h"

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TAILLE_MESSAGE 8192
#define NOMBRE_VALEURS_MAX 31
#define TAILLE_VALEUR_MAX 1024

const char *svg_file_path = "pie_chart.svg";
static int socketfd = -1;

static void ignorer_espaces(const char **curseur)
{
    while (**curseur == ' ' || **curseur == '\t'
           || **curseur == '\r' || **curseur == '\n')
        (*curseur)++;
}

static int valeur_hexadecimale(char caractere)
{
    if (caractere >= '0' && caractere <= '9')
        return caractere - '0';
    if (caractere >= 'a' && caractere <= 'f')
        return caractere - 'a' + 10;
    if (caractere >= 'A' && caractere <= 'F')
        return caractere - 'A' + 10;
    return -1;
}

static int lire_code_unicode(const char **curseur, unsigned int *code)
{
    unsigned int valeur = 0;
    for (int i = 0; i < 4; i++)
    {
        if ((*curseur)[i] == '\0')
            return 0;
        int chiffre = valeur_hexadecimale((*curseur)[i]);
        if (chiffre < 0)
            return 0;
        valeur = valeur * 16U + (unsigned int)chiffre;
    }
    *curseur += 4;
    *code = valeur;
    return 1;
}

static int ajouter_utf8(char *sortie, size_t capacite, size_t *longueur,
                        unsigned int code)
{
    unsigned char octets[4];
    size_t nombre;
    if (code == 0 || code > 0x10ffff
        || (code >= 0xd800 && code <= 0xdfff))
        return 0;
    if (code <= 0x7f)
    {
        octets[0] = (unsigned char)code;
        nombre = 1;
    }
    else if (code <= 0x7ff)
    {
        octets[0] = (unsigned char)(0xc0 | (code >> 6));
        octets[1] = (unsigned char)(0x80 | (code & 0x3f));
        nombre = 2;
    }
    else if (code <= 0xffff)
    {
        octets[0] = (unsigned char)(0xe0 | (code >> 12));
        octets[1] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        octets[2] = (unsigned char)(0x80 | (code & 0x3f));
        nombre = 3;
    }
    else
    {
        octets[0] = (unsigned char)(0xf0 | (code >> 18));
        octets[1] = (unsigned char)(0x80 | ((code >> 12) & 0x3f));
        octets[2] = (unsigned char)(0x80 | ((code >> 6) & 0x3f));
        octets[3] = (unsigned char)(0x80 | (code & 0x3f));
        nombre = 4;
    }
    if (*longueur + nombre >= capacite)
        return 0;
    memcpy(sortie + *longueur, octets, nombre);
    *longueur += nombre;
    sortie[*longueur] = '\0';
    return 1;
}

static int lire_chaine_json(const char **curseur, char *sortie,
                            size_t capacite)
{
    ignorer_espaces(curseur);
    if (**curseur != '"')
        return 0;
    (*curseur)++;
    size_t longueur = 0;
    sortie[0] = '\0';

    while (**curseur != '\0')
    {
        unsigned char caractere = (unsigned char)*(*curseur)++;
        if (caractere == '"')
            return 1;
        if (caractere < 0x20)
            return 0;
        if (caractere == '\\')
        {
            caractere = (unsigned char)*(*curseur)++;
            switch (caractere)
            {
                case '"': case '\\': case '/':
                    break;
                case 'b': caractere = '\b'; break;
                case 'f': caractere = '\f'; break;
                case 'n': caractere = '\n'; break;
                case 'r': caractere = '\r'; break;
                case 't': caractere = '\t'; break;
                case 'u':
                {
                    unsigned int code;
                    if (!lire_code_unicode(curseur, &code))
                        return 0;
                    if (code >= 0xd800 && code <= 0xdbff)
                    {
                        if ((*curseur)[0] != '\\' || (*curseur)[1] != 'u')
                            return 0;
                        *curseur += 2;
                        unsigned int bas;
                        if (!lire_code_unicode(curseur, &bas)
                            || bas < 0xdc00 || bas > 0xdfff)
                            return 0;
                        code = 0x10000 + ((code - 0xd800) << 10)
                             + (bas - 0xdc00);
                    }
                    if (!ajouter_utf8(sortie, capacite, &longueur, code))
                        return 0;
                    continue;
                }
                default:
                    return 0;
            }
        }

        if (longueur + 1 >= capacite)
            return 0;
        sortie[longueur++] = (char)caractere;
        sortie[longueur] = '\0';
    }
    return 0;
}

static int analyser_json(const char *json, char *code, size_t taille_code,
                         char valeurs[NOMBRE_VALEURS_MAX][TAILLE_VALEUR_MAX],
                         size_t *nombre_valeurs)
{
    const char *curseur = json;
    int code_present = 0;
    int valeurs_presentes = 0;
    *nombre_valeurs = 0;

    ignorer_espaces(&curseur);
    if (*curseur++ != '{')
        return 0;

    for (;;)
    {
        char cle[32];
        ignorer_espaces(&curseur);
        if (*curseur == '}')
        {
            curseur++;
            break;
        }
        if (!lire_chaine_json(&curseur, cle, sizeof(cle)))
            return 0;
        ignorer_espaces(&curseur);
        if (*curseur++ != ':')
            return 0;

        if (strcmp(cle, "code") == 0)
        {
            if (code_present || !lire_chaine_json(&curseur, code, taille_code))
                return 0;
            code_present = 1;
        }
        else if (strcmp(cle, "valeurs") == 0)
        {
            if (valeurs_presentes)
                return 0;
            ignorer_espaces(&curseur);
            if (*curseur++ != '[')
                return 0;
            ignorer_espaces(&curseur);
            if (*curseur != ']')
            {
                for (;;)
                {
                    if (*nombre_valeurs >= NOMBRE_VALEURS_MAX
                        || !lire_chaine_json(&curseur,
                            valeurs[*nombre_valeurs], TAILLE_VALEUR_MAX))
                        return 0;
                    (*nombre_valeurs)++;
                    ignorer_espaces(&curseur);
                    if (*curseur == ']')
                        break;
                    if (*curseur++ != ',')
                        return 0;
                }
            }
            curseur++;
            valeurs_presentes = 1;
        }
        else
            return 0;

        ignorer_espaces(&curseur);
        if (*curseur == '}')
        {
            curseur++;
            break;
        }
        if (*curseur++ != ',')
            return 0;
    }

    ignorer_espaces(&curseur);
    return *curseur == '\0' && code_present && valeurs_presentes;
}

static int ajouter_chaine_json(char *json, size_t capacite, size_t *utilise,
                               const char *chaine)
{
    if (capacite - *utilise < 2)
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
            int longueur = snprintf(json + *utilise, capacite - *utilise,
                                    "\\u%04x", (unsigned int)*caractere);
            if (longueur != 6)
                return 0;
            *utilise += (size_t)longueur;
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

static int creer_reponse(char *json, size_t capacite, const char *code,
                         const char *valeur)
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
    if (!ajouter_chaine_json(json, capacite, &utilise, valeur)
        || capacite - utilise < 3)
        return 0;
    memcpy(json + utilise, "]}", 3);
    return 1;
}

static int envoyer_tout(int client_socket_fd, const char *donnees,
                        size_t longueur)
{
    size_t envoyes = 0;
    while (envoyes < longueur)
    {
        ssize_t resultat = send(client_socket_fd, donnees + envoyes,
                                longueur - envoyes, MSG_NOSIGNAL);
        if (resultat < 0 && errno == EINTR)
            continue;
        if (resultat <= 0)
        {
            if (resultat < 0)
                perror("send");
            return 0;
        }
        envoyes += (size_t)resultat;
    }
    return 1;
}

static int envoyer_ligne(int client_socket_fd, const char *message)
{
    size_t longueur = strlen(message);
    return envoyer_tout(client_socket_fd, message, longueur)
        && envoyer_tout(client_socket_fd, "\n", 1);
}

static int recevoir_ligne(int client_socket_fd, char *message, size_t capacite)
{
    size_t longueur = 0;
    while (longueur + 1 < capacite)
    {
        char caractere;
        ssize_t resultat = recv(client_socket_fd, &caractere, 1, 0);
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

static int envoyer_reponse(int socket_client, const char *code,
                           const char *valeur)
{
    char reponse[TAILLE_MESSAGE];
    if (!creer_reponse(reponse, sizeof(reponse), code, valeur))
    {
        fprintf(stderr, "Impossible de créer la réponse JSON.\n");
        return 0;
    }
    return envoyer_ligne(socket_client, reponse);
}

static int convertir_entier(const char *texte, int *valeur)
{
    char *fin;
    errno = 0;
    long resultat = strtol(texte, &fin, 10);
    if (errno != 0 || fin == texte || *fin != '\0'
        || resultat < INT_MIN || resultat > INT_MAX)
        return 0;
    *valeur = (int)resultat;
    return 1;
}

static int calculer(char operateur, int num1, int num2, int *resultat)
{
    long long valeur;
    switch (operateur)
    {
        case '+': valeur = (long long)num1 + num2; break;
        case '-': valeur = (long long)num1 - num2; break;
        case '*': valeur = (long long)num1 * num2; break;
        case '/':
            if (num2 == 0 || (num1 == INT_MIN && num2 == -1))
                return 0;
            valeur = num1 / num2;
            break;
        case '%':
            if (num2 == 0)
                return 0;
            valeur = num1 == INT_MIN && num2 == -1 ? 0 : num1 % num2;
            break;
        case '&': valeur = num1 & num2; break;
        case '|': valeur = num1 | num2; break;
        case '~': valeur = ~num1; break;
        default: return 0;
    }
    if (valeur < INT_MIN || valeur > INT_MAX)
        return 0;
    *resultat = (int)valeur;
    return 1;
}

static int traiter_calcul(int socket_client, char valeurs[][TAILLE_VALEUR_MAX],
                          size_t nombre_valeurs)
{
    int num1;
    int num2;
    int resultat;
    if (nombre_valeurs != 3 || strlen(valeurs[0]) != 1
        || !convertir_entier(valeurs[1], &num1)
        || !convertir_entier(valeurs[2], &num2)
        || !calculer(valeurs[0][0], num1, num2, &resultat))
        return envoyer_reponse(socket_client, "erreur",
                               "Opération invalide ou résultat hors limites.");

    char texte[32];
    (void)snprintf(texte, sizeof(texte), "%d", resultat);
    return envoyer_reponse(socket_client, "calcule", texte);
}

static int couleur_valide(const char *couleur)
{
    if (strlen(couleur) != 7 || couleur[0] != '#')
        return 0;
    for (size_t i = 1; i < 7; i++)
    {
        if (valeur_hexadecimale(couleur[i]) < 0)
            return 0;
    }
    return 1;
}

static void visualiser_plot(void)
{
    if (getenv("DISPLAY") == NULL)
    {
        printf("Firefox non lancé : aucun affichage graphique disponible.\n");
        return;
    }

    pid_t enfant = fork();
    if (enfant == 0)
    {
        execlp("firefox", "firefox", svg_file_path, (char *)NULL);
        perror("firefox");
        _exit(127);
    }
    if (enfant < 0)
    {
        perror("fork");
        return;
    }

    int statut;
    while (waitpid(enfant, &statut, 0) < 0)
    {
        if (errno != EINTR)
        {
            perror("waitpid");
            return;
        }
    }
    if (!WIFEXITED(statut) || WEXITSTATUS(statut) != 0)
        fprintf(stderr, "Firefox n'a pas pu ouvrir le graphique SVG.\n");
}

static int plot(char couleurs[][TAILLE_VALEUR_MAX], size_t nombre)
{
    if (nombre == 0 || nombre > 30)
        return 0;
    for (size_t i = 0; i < nombre; i++)
    {
        if (!couleur_valide(couleurs[i]))
            return 0;
    }

    FILE *fichier = fopen(svg_file_path, "w");
    if (fichier == NULL)
    {
        perror(svg_file_path);
        return 0;
    }

    int ecriture_ok = fprintf(fichier,
        "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
        "<svg width=\"400\" height=\"400\" xmlns=\"http://www.w3.org/2000/svg\">\n"
        "  <rect width=\"100%%\" height=\"100%%\" fill=\"#ffffff\" />\n") >= 0;
    const double centre_x = 200.0;
    const double centre_y = 200.0;
    const double rayon = 150.0;

    if (nombre == 1 && ecriture_ok)
    {
        ecriture_ok = fprintf(fichier,
            "  <circle cx=\"%.2f\" cy=\"%.2f\" r=\"%.2f\" fill=\"%s\" />\n",
            centre_x, centre_y, rayon, couleurs[0]) >= 0;
    }
    else
    {
        double angle_depart = -90.0;
        for (size_t i = 0; i < nombre && ecriture_ok; i++)
        {
            double delta = 360.0 / (double)nombre;
            double angle_fin = angle_depart + delta;
            double radians_depart = angle_depart * 3.14159265358979323846 / 180.0;
            double radians_fin = angle_fin * 3.14159265358979323846 / 180.0;
            double x1 = centre_x + rayon * cos(radians_depart);
            double y1 = centre_y + rayon * sin(radians_depart);
            double x2 = centre_x + rayon * cos(radians_fin);
            double y2 = centre_y + rayon * sin(radians_fin);
            int grand_arc = delta > 180.0 ? 1 : 0;
            ecriture_ok = fprintf(fichier,
                "  <path d=\"M%.2f,%.2f A%.2f,%.2f 0 %d,1 %.2f,%.2f "
                "L%.2f,%.2f Z\" fill=\"%s\" />\n",
                x1, y1, rayon, rayon, grand_arc, x2, y2,
                centre_x, centre_y, couleurs[i]) >= 0;
            angle_depart = angle_fin;
        }
    }

    if (fprintf(fichier, "</svg>\n") < 0)
        ecriture_ok = 0;
    if (fclose(fichier) != 0)
    {
        perror("fclose");
        ecriture_ok = 0;
    }
    if (!ecriture_ok)
    {
        fprintf(stderr, "Erreur lors de la génération du graphique SVG.\n");
        return 0;
    }

    printf("Graphique SVG créé : %s\n", svg_file_path);
    visualiser_plot();
    return 1;
}

static int traiter_couleurs(int socket_client,
                            char valeurs[][TAILLE_VALEUR_MAX],
                            size_t nombre_valeurs)
{
    int nombre;
    if (nombre_valeurs < 2 || !convertir_entier(valeurs[0], &nombre)
        || nombre < 1 || nombre > 30
        || nombre_valeurs != (size_t)nombre + 1)
        return envoyer_reponse(socket_client, "erreur",
                               "Nombre de couleurs invalide.");

    if (!plot(valeurs + 1, (size_t)nombre))
        return envoyer_reponse(socket_client, "erreur",
                               "Couleurs invalides ou génération SVG impossible.");
    return envoyer_reponse(socket_client, "couleurs", "Graphique SVG créé.");
}

int recois_envoie_message(int client_socket_fd, const char *message)
{
    char code[32];
    char valeurs[NOMBRE_VALEURS_MAX][TAILLE_VALEUR_MAX];
    size_t nombre_valeurs;

    if (!analyser_json(message, code, sizeof(code), valeurs, &nombre_valeurs))
        return envoyer_reponse(client_socket_fd, "erreur", "JSON invalide.");

    if (strcmp(code, "message") == 0)
    {
        if (nombre_valeurs != 1)
            return envoyer_reponse(client_socket_fd, "erreur",
                                   "Le message doit contenir une valeur.");
        return envoyer_reponse(client_socket_fd, "message", valeurs[0]);
    }
    if (strcmp(code, "calcule") == 0)
        return traiter_calcul(client_socket_fd, valeurs, nombre_valeurs);
    if (strcmp(code, "couleurs") == 0)
        return traiter_couleurs(client_socket_fd, valeurs, nombre_valeurs);
    return envoyer_reponse(client_socket_fd, "erreur", "Code inconnu.");
}

static void gestionnaire_ctrl_c(int signal_recu)
{
    (void)signal_recu;
    static const char message[] = "\nSignal Ctrl+C capturé. Sortie du programme.\n";
    (void)write(STDOUT_FILENO, message, sizeof(message) - 1);
    if (socketfd != -1)
        (void)close(socketfd);
    _exit(EXIT_SUCCESS);
}

int main(void)
{
    (void)setvbuf(stdout, NULL, _IONBF, 0);
    socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if (socketfd < 0)
    {
        perror("socket");
        return EXIT_FAILURE;
    }

    int option = 1;
    if (setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR,
                   &option, sizeof(option)) < 0)
    {
        perror("setsockopt");
        close(socketfd);
        return EXIT_FAILURE;
    }

    struct sockaddr_in adresse;
    memset(&adresse, 0, sizeof(adresse));
    adresse.sin_family = AF_INET;
    adresse.sin_port = htons(PORT);
    adresse.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(socketfd, (struct sockaddr *)&adresse, sizeof(adresse)) < 0)
    {
        perror("bind");
        close(socketfd);
        return EXIT_FAILURE;
    }
    if (listen(socketfd, 10) < 0)
    {
        perror("listen");
        close(socketfd);
        return EXIT_FAILURE;
    }

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = gestionnaire_ctrl_c;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, NULL) < 0)
    {
        perror("sigaction");
        close(socketfd);
        return EXIT_FAILURE;
    }

    printf("Serveur en attente de connexions...\n");
    for (;;)
    {
        struct sockaddr_in client_adresse;
        socklen_t longueur_adresse = sizeof(client_adresse);
        int client_socket = accept(socketfd,
                                   (struct sockaddr *)&client_adresse,
                                   &longueur_adresse);
        if (client_socket < 0)
        {
            if (errno == EINTR)
                continue;
            perror("accept");
            continue;
        }

        char message[TAILLE_MESSAGE];
        for (;;)
        {
            int statut = recevoir_ligne(client_socket, message, sizeof(message));
            if (statut == 0)
                break;
            if (statut < 0)
            {
                fprintf(stderr, "Réception d'un message invalide.\n");
                break;
            }
            printf("Message reçu : %s\n", message);
            if (!recois_envoie_message(client_socket, message))
                break;
        }
        if (close(client_socket) != 0)
            perror("close");
    }
}
