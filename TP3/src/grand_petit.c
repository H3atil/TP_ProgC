#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(void)
{
    int nombres[100];

    srand((unsigned int)time(NULL));
    for (int i = 0; i < 100; i++)
        nombres[i] = rand() % 1000 + 1;

    int plus_petit = nombres[0];
    int plus_grand = nombres[0];
    for (int i = 1; i < 100; i++)
    {
        if (nombres[i] < plus_petit)
            plus_petit = nombres[i];
        if (nombres[i] > plus_grand)
            plus_grand = nombres[i];
    }

    printf("Le numéro le plus grand est : %d\n", plus_grand);
    printf("Le numéro le plus petit est : %d\n", plus_petit);
    return 0;
}