#include <stdio.h>

int main(void)
{
    int nombres[100];
    int recherche;

    for (int i = 0; i < 100; i++)
        nombres[i] = i * 2 - 50;

    printf("Tableau trié :\n");
    for (int i = 0; i < 100; i++)
        printf("%d%c", nombres[i], i == 99 ? '\n' : ' ');

    printf("Entrez l'entier que vous souhaitez chercher : ");
    if (scanf("%d", &recherche) != 1)
    {
        fprintf(stderr, "Entree invalide.\n");
        return 1;
    }

    int debut = 0;
    int fin = 99;
    int trouve = 0;
    while (debut <= fin)
    {
        int milieu = debut + (fin - debut) / 2;
        if (nombres[milieu] == recherche)
        {
            trouve = 1;
            break;
        }
        if (nombres[milieu] < recherche)
            debut = milieu + 1;
        else
            fin = milieu - 1;
    }

    printf("Résultat : entier %s\n", trouve ? "présent" : "absent");
    return 0;
}