#include "fichier.h"

#include <stdio.h>

struct etudiant
{
    char nom[64];
    char prenom[64];
    char adresse[192];
    double note1;
    double note2;
};

static int lire_texte(const char *invite, char *texte, size_t capacite)
{
    printf("%s", invite);
    if (fgets(texte, (int)capacite, stdin) == NULL)
        return 0;

    for (size_t i = 0; texte[i] != '\0'; i++)
    {
        if (texte[i] == '\n')
        {
            texte[i] = '\0';
            break;
        }
    }
    return 1;
}

static void vider_ligne(void)
{
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF)
        ;
}

int main(void)
{
    struct etudiant etudiants[5];
    char ligne[512];

    for (int i = 0; i < 5; i++)
    {
        printf("Entrez les détails de l'étudiant.e %d :\n", i + 1);
        if (!lire_texte("Nom : ", etudiants[i].nom, sizeof(etudiants[i].nom))
            || !lire_texte("Prénom : ", etudiants[i].prenom, sizeof(etudiants[i].prenom))
            || !lire_texte("Adresse : ", etudiants[i].adresse, sizeof(etudiants[i].adresse)))
        {
            fprintf(stderr, "Lecture des informations impossible.\n");
            return 1;
        }

        printf("Note 1 : ");
        if (scanf("%lf", &etudiants[i].note1) != 1)
        {
            fprintf(stderr, "Note invalide.\n");
            return 1;
        }
        printf("Note 2 : ");
        if (scanf("%lf", &etudiants[i].note2) != 1)
        {
            fprintf(stderr, "Note invalide.\n");
            return 1;
        }
        vider_ligne();

        int longueur = snprintf(ligne, sizeof(ligne),
                                "%s;%s;%s;%.2f;%.2f",
                                etudiants[i].nom, etudiants[i].prenom,
                                etudiants[i].adresse, etudiants[i].note1,
                                etudiants[i].note2);
        if (longueur < 0 || (size_t)longueur >= sizeof(ligne)
            || !ecrire_dans_fichier("etudiant.txt", ligne))
        {
            fprintf(stderr, "Enregistrement de l'étudiant impossible.\n");
            return 1;
        }
    }

    printf("Les détails des étudiants ont été enregistrés dans le fichier etudiant.txt.\n");
    return 0;
}