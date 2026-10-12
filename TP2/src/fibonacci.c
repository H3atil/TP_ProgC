#include <stdio.h>

int main(void)
{
    int n;
    unsigned long long precedent = 0;
    unsigned long long courant = 1;

    printf("Entrez n (0 a 93) : ");
    if (scanf("%d", &n) != 1 || n < 0 || n > 93)
    {
        fprintf(stderr, "n doit etre un entier compris entre 0 et 93.\n");
        return 1;
    }
    printf("\n");

    for (int i = 0; i <= n; i++)
    {
        unsigned long long terme;

        if (i == 0)
            terme = precedent;
        else if (i == 1)
            terme = courant;
        else
        {
            terme = precedent + courant;
            precedent = courant;
            courant = terme;
        }

        printf("%llu%s", terme, i == n ? "\n" : ", ");
    }

    return 0;
}