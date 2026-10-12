#include <stdio.h>

int main(void)
{
    int nombres[100];
    int recherche;
    int trouve = 0;

    for (int i = 0; i < 100; i++)
        nombres[i] = i + 1;

    printf("Tableau :\n");
    for (int i = 0; i < 100; i++)
        printf("%d%c", nombres[i], i == 99 ? '\n' : ' ');

    printf("Entrez l'entier que vous souhaitez chercher : ");
    if (scanf("%d", &recherche) != 1)
    {
        fprintf(stderr, "Entree invalide.\n");
        return 1;
    }

    for (int i = 0; i < 100; i++)
    {
        if (nombres[i] == recherche)
        {
            trouve = 1;
            break;
        }
    }

    printf("Résultat : entier %s\n", trouve ? "présent" : "absent");
    return 0;
}