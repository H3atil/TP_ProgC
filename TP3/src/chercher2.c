#include <stdio.h>

int main(void)
{
    const char *phrases[10] = {
        "Bonjour, comment ça va ?",
        "Le temps est magnifique aujourd'hui.",
        "C'est une belle journée.",
        "La programmation en C est amusante.",
        "Les tableaux en C sont puissants.",
        "Les pointeurs en C peuvent être déroutants.",
        "Il fait beau dehors.",
        "La recherche dans un tableau est intéressante.",
        "Les structures de données sont importantes.",
        "Programmer en C, c'est génial."
    };
    char recherche[256];

    printf("Entrez la phrase à rechercher : ");
    if (fgets(recherche, sizeof(recherche), stdin) == NULL)
    {
        fprintf(stderr, "Lecture de la phrase impossible.\n");
        return 1;
    }

    for (int i = 0; recherche[i] != '\0'; i++)
    {
        if (recherche[i] == '\n')
        {
            recherche[i] = '\0';
            break;
        }
    }

    int trouve = 0;
    for (int i = 0; i < 10; i++)
    {
        int j = 0;
        while (phrases[i][j] != '\0'
               && recherche[j] != '\0'
               && phrases[i][j] == recherche[j])
            j++;

        if (phrases[i][j] == '\0' && recherche[j] == '\0')
        {
            trouve = 1;
            break;
        }
    }

    puts(trouve ? "Phrase trouvée" : "Phrase non trouvée");
    return 0;
}
