#include <stdio.h>
#include <stdlib.h>

static int lire_ligne(const char *invite, char *tampon, size_t capacite)
{
    printf("%s", invite);
    if (fgets(tampon, (int)capacite, stdin) == NULL)
        return 0;
    for (size_t i = 0; tampon[i] != '\0'; i++)
    {
        if (tampon[i] == '\n')
        {
            tampon[i] = '\0';
            break;
        }
    }
    return 1;
}

static size_t compter_occurrences(const char *ligne, const char *phrase)
{
    size_t occurrences = 0;
    for (size_t i = 0; ligne[i] != '\0'; i++)
    {
        size_t j = 0;
        while (phrase[j] != '\0' && ligne[i + j] != '\0'
               && ligne[i + j] == phrase[j])
            j++;
        if (phrase[j] == '\0')
            occurrences++;
    }
    return occurrences;
}

static int lire_ligne_fichier(FILE *fichier, char **ligne, size_t *capacite,
                              size_t *longueur)
{
    *longueur = 0;
    int caractere;
    while ((caractere = fgetc(fichier)) != EOF)
    {
        if (*longueur + 1 >= *capacite)
        {
            size_t nouvelle_capacite = *capacite == 0 ? 256 : *capacite * 2;
            if (nouvelle_capacite <= *capacite)
                return -1;
            char *nouvelle_ligne = realloc(*ligne, nouvelle_capacite);
            if (nouvelle_ligne == NULL)
            {
                perror("realloc");
                return -1;
            }
            *ligne = nouvelle_ligne;
            *capacite = nouvelle_capacite;
        }

        (*ligne)[(*longueur)++] = (char)caractere;
        if (caractere == '\n')
            break;
    }

    if (ferror(fichier))
        return -1;
    if (*longueur == 0 && caractere == EOF)
        return 0;
    (*ligne)[*longueur] = '\0';
    return 1;
}

int main(int argc, char **argv)
{
    char nom[256];
    char phrase[256];

    if (argc > 2)
    {
        fprintf(stderr, "Usage : %s [fichier]\n", argv[0]);
        return 1;
    }
    if (argc == 2)
    {
        size_t i = 0;
        while (argv[1][i] != '\0' && i + 1 < sizeof(nom))
        {
            nom[i] = argv[1][i];
            i++;
        }
        if (argv[1][i] != '\0')
        {
            fprintf(stderr, "Nom de fichier trop long.\n");
            return 1;
        }
        nom[i] = '\0';
    }
    else if (!lire_ligne("Entrez le nom du fichier : ", nom, sizeof(nom)))
    {
        fprintf(stderr, "Lecture du nom de fichier impossible.\n");
        return 1;
    }

    if (!lire_ligne("Entrez la phrase que vous souhaitez rechercher : ",
                    phrase, sizeof(phrase)))
    {
        fprintf(stderr, "Lecture de la phrase impossible.\n");
        return 1;
    }
    if (phrase[0] == '\0')
    {
        fprintf(stderr, "La phrase à rechercher ne peut pas être vide.\n");
        return 1;
    }

    FILE *fichier = fopen(nom, "r");
    if (fichier == NULL)
    {
        perror(nom);
        return 1;
    }

    printf("Résultats de la recherche :\n");
    char *ligne = NULL;
    size_t capacite = 0;
    size_t numero_ligne = 0;
    int lecture;
    for (;;)
    {
        size_t longueur;
        lecture = lire_ligne_fichier(fichier, &ligne, &capacite, &longueur);
        if (lecture <= 0)
            break;
        numero_ligne++;
        size_t occurrences = compter_occurrences(ligne, phrase);
        if (occurrences > 0)
            printf("Ligne %zu, %zu fois\n", numero_ligne, occurrences);
    }

    free(ligne);
    if (lecture < 0)
    {
        perror("Erreur de lecture");
        fclose(fichier);
        return 1;
    }
    if (fclose(fichier) != 0)
    {
        perror("Erreur de fermeture");
        return 1;
    }
    return 0;
}