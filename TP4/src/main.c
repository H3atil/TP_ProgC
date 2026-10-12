
#include "fichier.h"
#include "liste.h"
#include "operator.h"

#include <stdio.h>

static int lire_chaine(const char *invite, char *destination, size_t capacite)
{
    printf("%s", invite);
    if (fgets(destination, (int)capacite, stdin) == NULL)
        return 0;

    for (size_t i = 0; destination[i] != '\0'; i++)
    {
        if (destination[i] == '\n')
        {
            destination[i] = '\0';
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

static int exercice_calcul(void)
{
    int num1;
    int num2;
    int resultat;
    char op;

    printf("Entrez num1 : ");
    if (scanf("%d", &num1) != 1)
        return 0;
    printf("Entrez num2 : ");
    if (scanf("%d", &num2) != 1)
        return 0;
    printf("Entrez l'operateur (+, -, *, /, %%, &, |, ~) : ");
    if (scanf(" %c", &op) != 1)
        return 0;

    if (!calculer(num1, num2, op, &resultat))
    {
        fprintf(stderr, "Operateur invalide ou division par zero.\n");
        return 0;
    }
    printf("Résultat : %d\n", resultat);
    return 1;
}

static int exercice_fichier(void)
{
    int choix;
    char nom[256];
    char message[1024];

    printf("Que souhaitez-vous faire ?\n1. Lire un fichier\n2. Ecrire dans un fichier\nVotre choix : ");
    if (scanf("%d", &choix) != 1)
        return 0;
    vider_ligne();

    if (choix == 1)
    {
        if (!lire_chaine("Entrez le nom du fichier à lire : ", nom, sizeof(nom)))
            return 0;
        printf("Contenu du fichier %s :\n", nom);
        return lire_fichier(nom);
    }
    if (choix == 2)
    {
        if (!lire_chaine("Entrez le nom du fichier dans lequel vous souhaitez écrire : ",
                         nom, sizeof(nom))
            || !lire_chaine("Entrez le message à écrire : ", message, sizeof(message)))
            return 0;
        if (!ecrire_dans_fichier(nom, message))
            return 0;
        printf("Le message a été écrit dans le fichier %s.\n", nom);
        return 1;
    }

    fprintf(stderr, "Choix invalide.\n");
    return 0;
}

static int exercice_liste(void)
{
    const struct couleur couleurs[10] = {
        {0xff, 0x00, 0x00, 0xff},
        {0x00, 0xff, 0x00, 0xff},
        {0x00, 0x00, 0xff, 0xff},
        {0xff, 0xff, 0x00, 0xff},
        {0xff, 0x00, 0xff, 0xff},
        {0x00, 0xff, 0xff, 0xff},
        {0x80, 0x80, 0x80, 0xff},
        {0xff, 0xa5, 0x00, 0xff},
        {0x80, 0x00, 0x80, 0xff},
        {0x00, 0x80, 0x80, 0xff}
    };
    struct liste_couleurs ma_liste;
    init_liste(&ma_liste);

    for (size_t i = 0; i < sizeof(couleurs) / sizeof(couleurs[0]); i++)
    {
        if (!insertion(&couleurs[i], &ma_liste))
        {
            detruire_liste(&ma_liste);
            fprintf(stderr, "Allocation mémoire impossible.\n");
            return 0;
        }
    }

    printf("Liste des couleurs :\n");
    parcours(&ma_liste);
    detruire_liste(&ma_liste);
    return 1;
}

int main(void)
{
    int exercice;

    printf("Choisissez l'exercice (1: calcul, 2: fichiers, 7: liste de couleurs) : ");
    if (scanf("%d", &exercice) != 1)
    {
        fprintf(stderr, "Choix invalide.\n");
        return 1;
    }
    vider_ligne();

    switch (exercice)
    {
        case 1:
            return exercice_calcul() ? 0 : 1;
        case 2:
            return exercice_fichier() ? 0 : 1;
        case 7:
            return exercice_liste() ? 0 : 1;
        default:
            fprintf(stderr, "Exercice inconnu.\n");
            return 1;
    }
}
