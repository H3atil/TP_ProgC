#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    int entiers[11];
    float flottants[11];
    int *p_entier;
    float *p_flottant;
    const int longueur = 11;

    srand((unsigned int)time(NULL));

    for (p_entier = entiers; p_entier < entiers + longueur; p_entier++)
        *p_entier = rand() % 124;
    for (p_flottant = flottants; p_flottant < flottants + longueur; p_flottant++)
        *p_flottant = (float)(rand() % 1000) / 100.0f;

    printf("Tableau d'entiers (avant la multiplication par 3) :\n");
    for (p_entier = entiers; p_entier < entiers + longueur; p_entier++)
        printf("%s%d", p_entier == entiers ? "" : ", ", *p_entier);
    printf("\n\nTableau de nombres a virgule flottante (avant la multiplication par 3) :\n");
    for (p_flottant = flottants; p_flottant < flottants + longueur; p_flottant++)
        printf("%s%.2f", p_flottant == flottants ? "" : ", ", (double)*p_flottant);

    for (p_entier = entiers; p_entier < entiers + longueur; p_entier++)
    {
        if ((p_entier - entiers) % 2 == 0)
            *p_entier *= 3;
    }
    for (p_flottant = flottants; p_flottant < flottants + longueur; p_flottant++)
    {
        if ((p_flottant - flottants) % 2 == 0)
            *p_flottant *= 3.0f;
    }

    printf("\n\nTableau d'entiers (apres la multiplication par 3) :\n");
    for (p_entier = entiers; p_entier < entiers + longueur; p_entier++)
        printf("%s%d", p_entier == entiers ? "" : ", ", *p_entier);
    printf("\n\nTableau de nombres a virgule flottante (apres la multiplication par 3) :\n");
    for (p_flottant = flottants; p_flottant < flottants + longueur; p_flottant++)
        printf("%s%.2f", p_flottant == flottants ? "" : ", ", (double)*p_flottant);
    printf("\n");

    return 0;
}