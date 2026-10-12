#include "serveur.h"

#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static int socketfd = -1;

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
            return -1;
        }
        envoyes += (size_t)resultat;
    }
    return 0;
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

int renvoie_message(int client_socket_fd, const char *message)
{
    size_t longueur = strlen(message);
    if (envoyer_tout(client_socket_fd, message, longueur) != 0
        || envoyer_tout(client_socket_fd, "\n", 1) != 0)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

int recois_numeros_calcule(int client_socket_fd, const char *message)
{
    char operateur;
    char surplus;
    int num1;
    int num2;
    int resultat;

    if (sscanf(message, "calcule : %c %d %d %c",
               &operateur, &num1, &num2, &surplus) != 3)
        return renvoie_message(client_socket_fd, "erreur : format de calcul invalide");

    long long calcul;
    switch (operateur)
    {
        case '+':
            calcul = (long long)num1 + num2;
            break;
        case '-':
            calcul = (long long)num1 - num2;
            break;
        case '*':
            calcul = (long long)num1 * num2;
            break;
        case '/':
            if (num2 == 0)
                return renvoie_message(client_socket_fd, "erreur : division par zero");
            if (num1 == INT_MIN && num2 == -1)
                return renvoie_message(client_socket_fd, "erreur : resultat hors limites");
            calcul = num1 / num2;
            break;
        case '%':
            if (num2 == 0)
                return renvoie_message(client_socket_fd, "erreur : modulo par zero");
            if (num1 == INT_MIN && num2 == -1)
                calcul = 0;
            else
                calcul = num1 % num2;
            break;
        case '&':
            resultat = num1 & num2;
            calcul = resultat;
            break;
        case '|':
            resultat = num1 | num2;
            calcul = resultat;
            break;
        case '~':
            resultat = ~num1;
            calcul = resultat;
            break;
        default:
            return renvoie_message(client_socket_fd, "erreur : operateur inconnu");
    }

    if (calcul < INT_MIN || calcul > INT_MAX)
        return renvoie_message(client_socket_fd, "erreur : resultat hors limites");

    char reponse[128];
    int longueur = snprintf(reponse, sizeof(reponse), "calcule : %d", (int)calcul);
    if (longueur < 0 || (size_t)longueur >= sizeof(reponse))
        return EXIT_FAILURE;
    return renvoie_message(client_socket_fd, reponse);
}

static int recois_envoie_message(int client_socket_fd, const char *message)
{
    printf("Message reçu: %s\n", message);

    if (strncmp(message, "message: ", 9) == 0)
    {
        char reponse[1024];
        printf("Message à envoyer au client : ");
        if (fgets(reponse, sizeof(reponse), stdin) == NULL)
        {
            fprintf(stderr, "Lecture de la réponse impossible.\n");
            return EXIT_FAILURE;
        }

        size_t longueur = strlen(reponse);
        if (longueur > 0 && reponse[longueur - 1] == '\n')
            reponse[longueur - 1] = '\0';
        else
        {
            int caractere;
            while ((caractere = getchar()) != '\n' && caractere != EOF)
                ;
        }

        char message_reponse[1050];
        int taille = snprintf(message_reponse, sizeof(message_reponse),
                              "message: %s", reponse);
        if (taille < 0 || (size_t)taille >= sizeof(message_reponse))
        {
            fprintf(stderr, "Réponse trop longue.\n");
            return EXIT_FAILURE;
        }
        return renvoie_message(client_socket_fd, message_reponse);
    }

    if (strncmp(message, "calcule :", 9) == 0)
        return recois_numeros_calcule(client_socket_fd, message);

    return renvoie_message(client_socket_fd, "erreur : message inconnu");
}

static void gerer_client(int client_socket_fd)
{
    char message[1024];
    for (;;)
    {
        int statut = recevoir_ligne(client_socket_fd, message, sizeof(message));
        if (statut == 0)
        {
            printf("Client déconnecté.\n");
            break;
        }
        if (statut < 0)
        {
            fprintf(stderr, "Réception d'un message invalide.\n");
            break;
        }
        if (recois_envoie_message(client_socket_fd, message) != EXIT_SUCCESS)
            break;
    }
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

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
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
    (void)signal(SIGCHLD, SIG_IGN);

    printf("Serveur en attente de connexions...\n");
    for (;;)
    {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket_fd = accept(socketfd,
                                      (struct sockaddr *)&client_addr,
                                      &client_addr_len);
        if (client_socket_fd < 0)
        {
            if (errno == EINTR)
                continue;
            perror("accept");
            continue;
        }

        pid_t child_pid = fork();
        if (child_pid == 0)
        {
            close(socketfd);
            socketfd = -1;
            gerer_client(client_socket_fd);
            close(client_socket_fd);
            exit(EXIT_SUCCESS);
        }
        if (child_pid < 0)
        {
            perror("fork");
            close(client_socket_fd);
        }
        else
            close(client_socket_fd);
    }
}
