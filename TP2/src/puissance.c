#include <stdio.h>

int main(void)
{
    int a = 2;
    int b = 3;
    int resultat = 1;

    for (int i = 0; i < b; i++)
        resultat *= a;

    printf("%d^%d = %d\n", a, b, resultat);
    return 0;
}