#include <stdio.h>

static void afficher_octets(const char *nom, const void *adresse, size_t taille)
{
    const unsigned char *octets = adresse;

    printf("Octets de %s :\n", nom);
    for (size_t i = 0; i < taille; i++)
        printf(" %02x", (unsigned int)octets[i]);
    printf("\n\n");
}

int main(void)
{
    short valeur_short = 0x0203;
    int valeur_int = 0x01020304;
    long int valeur_long = 0x0102030405060708L;
    float valeur_float = 1.0f;
    double valeur_double = 1.0;
    long double valeur_long_double = 1.0L;

    afficher_octets("short", &valeur_short, sizeof(valeur_short));
    afficher_octets("int", &valeur_int, sizeof(valeur_int));
    afficher_octets("long int", &valeur_long, sizeof(valeur_long));
    afficher_octets("float", &valeur_float, sizeof(valeur_float));
    afficher_octets("double", &valeur_double, sizeof(valeur_double));
    afficher_octets("long double", &valeur_long_double, sizeof(valeur_long_double));

    return 0;
}