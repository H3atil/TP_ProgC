#include "fichier.h"

#include <stdio.h>

int lire_fichier(const char *nom_de_fichier)
{
    FILE *fichier = fopen(nom_de_fichier, "r");
    if (fichier == NULL)
    {
        perror(nom_de_fichier);
        return 0;
    }

    char ligne[1024];
    while (fgets(ligne, sizeof(ligne), fichier) != NULL)
        fputs(ligne, stdout);

    if (ferror(fichier))
    {
        perror("Erreur de lecture");
        fclose(fichier);
        return 0;
    }

    if (fclose(fichier) != 0)
    {
        perror("Erreur de fermeture");
        return 0;
    }
    return 1;
}

int ecrire_dans_fichier(const char *nom_de_fichier, const char *message)
{
    FILE *fichier = fopen(nom_de_fichier, "a");
    if (fichier == NULL)
    {
        perror(nom_de_fichier);
        return 0;
    }

    size_t longueur = 0;
    while (message[longueur] != '\0')
        longueur++;

    if (fputs(message, fichier) == EOF
        || ((longueur == 0 || message[longueur - 1] != '\n')
            && fputc('\n', fichier) == EOF))
    {
        perror("Erreur d'ecriture");
        fclose(fichier);
        return 0;
    }

    if (fclose(fichier) != 0)
    {
        perror("Erreur de fermeture");
        return 0;
    }
    return 1;
}