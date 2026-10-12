#include <stdio.h>

int main(void)
{
    int nombres[100];

    for (int i = 0; i < 100; i++)
        nombres[i] = (i * 37) % 101 - 50;

    printf("Tableau non trié :\n");
    for (int i = 0; i < 100; i++)
        printf("%d%c", nombres[i], i == 99 ? '\n' : ' ');

    for (int i = 1; i < 100; i++)
    {
        int valeur = nombres[i];
        int j = i;
        while (j > 0 && nombres[j - 1] > valeur)
        {
            nombres[j] = nombres[j - 1];
            j--;
        }
        nombres[j] = valeur;
    }

    printf("Tableau trié par ordre croissant :\n");
    for (int i = 0; i < 100; i++)
        printf("%d%c", nombres[i], i == 99 ? '\n' : ' ');

    return 0;
}