#include <stdio.h>

int main(void)
{
    double rayon = 5.0;
    double pi = 3.14159;

    double aire = pi * rayon * rayon;
    double perimetre = 2 * pi * rayon;

    printf("Aire : %f\n", aire);
    printf("Perimetre : %f\n", perimetre);

    return 0;
}