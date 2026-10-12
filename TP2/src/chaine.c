#include <stdio.h>

int main(void)
{
    char premiere[] = "Hello";
    char deuxieme[] = " World!";
    char copie[sizeof(premiere)];
    char concatenee[sizeof(premiere) + sizeof(deuxieme) - 1];
    size_t longueur_premiere = 0;
    size_t longueur_deuxieme = 0;
    size_t i;

    while (premiere[longueur_premiere] != '\0')
        longueur_premiere++;
    while (deuxieme[longueur_deuxieme] != '\0')
        longueur_deuxieme++;

    for (i = 0; premiere[i] != '\0'; i++)
    {
        copie[i] = premiere[i];
        concatenee[i] = premiere[i];
    }
    copie[i] = '\0';

    for (i = 0; deuxieme[i] != '\0'; i++)
        concatenee[longueur_premiere + i] = deuxieme[i];
    concatenee[longueur_premiere + i] = '\0';

    printf("Longueur de \"%s\" : %zu\n", premiere, longueur_premiere);
    printf("Longueur de \"%s\" : %zu\n", deuxieme, longueur_deuxieme);
    printf("Longueur totale : %zu\n", longueur_premiere + longueur_deuxieme);
    printf("Copie : %s\n", copie);
    printf("Concaténation : %s\n", concatenee);

    return 0;
}