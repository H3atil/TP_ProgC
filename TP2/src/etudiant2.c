#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Etudiant
{
    char nom[32];
    char prenom[32];
    char adresse[96];
    float note_c;
    float note_systeme;
};

static int lire_ligne(const char *invite, char *destination, size_t capacite)
{
    printf("%s", invite);
    if (fgets(destination, (int)capacite, stdin) == NULL)
        return 0;

    size_t longueur = strlen(destination);
    if (longueur > 0 && destination[longueur - 1] == '\n')
        destination[longueur - 1] = '\0';
    else
    {
        int caractere = getchar();
        if (caractere != '\n' && caractere != EOF)
        {
            while ((caractere = getchar()) != '\n' && caractere != EOF)
                ;
            fprintf(stderr, "Saisie trop longue.\n");
            return 0;
        }
    }
    return 1;
}

static int lire_note(const char *invite, float *note)
{
    char texte[64];
    if (!lire_ligne(invite, texte, sizeof(texte)))
        return 0;

    char *fin;
    errno = 0;
    float valeur = strtof(texte, &fin);
    while (*fin == ' ' || *fin == '\t')
        fin++;
    if (errno == ERANGE || fin == texte || *fin != '\0')
    {
        fprintf(stderr, "Note invalide.\n");
        return 0;
    }
    *note = valeur;
    return 1;
}

int main(void)
{
    struct Etudiant etudiants[5];
    char nom[sizeof(etudiants[0].nom)];
    char prenom[sizeof(etudiants[0].prenom)];
    char adresse[sizeof(etudiants[0].adresse)];

    for (int i = 0; i < 5; i++)
    {
        printf("Entrez les détails de l'étudiant.e %d :\n", i + 1);
        if (!lire_ligne("Nom : ", nom, sizeof(nom))
            || !lire_ligne("Prénom : ", prenom, sizeof(prenom))
            || !lire_ligne("Adresse : ", adresse, sizeof(adresse))
            || !lire_note("Note 1 : ", &etudiants[i].note_c)
            || !lire_note("Note 2 : ", &etudiants[i].note_systeme))
        {
            fprintf(stderr, "Lecture des informations impossible.\n");
            return 1;
        }

        strcpy(etudiants[i].nom, nom);
        strcpy(etudiants[i].prenom, prenom);
        strcpy(etudiants[i].adresse, adresse);
    }

    for (int i = 0; i < 5; i++)
    {
        printf("Etudiant.e %d :\n", i + 1);
        printf("Nom : %s\n", etudiants[i].nom);
        printf("Prenom : %s\n", etudiants[i].prenom);
        printf("Adresse : %s\n", etudiants[i].adresse);
        printf("Note 1 : %.1f\n", etudiants[i].note_c);
        printf("Note 2 : %.1f\n\n", etudiants[i].note_systeme);
    }

    return 0;
}