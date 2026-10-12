#include <stdio.h>

static void afficher_variable(const char *nom, const void *adresse, size_t taille)
{
    const unsigned char *octets = adresse;

    printf("%s (adresse %p) : 0x", nom, adresse);
    for (size_t i = 0; i < taille; i++)
        printf("%02x", (unsigned int)octets[i]);
    printf("\n");
}

int main(void)
{
    char c = 'A';
    signed char sc = -100;
    unsigned char uc = 200;
    short s = -32000;
    unsigned short us = 60000;
    int i = -1000;
    unsigned int ui = 1000U;
    long int li = -100000L;
    unsigned long int uli = 100000UL;
    long long int lli = -1000000000LL;
    unsigned long long int ulli = 1000000000ULL;
    float f = 3.14f;
    double d = 3.14159;
    long double ld = 3.1415926535L;

    char *pc = &c;
    signed char *psc = &sc;
    unsigned char *puc = &uc;
    short *ps = &s;
    unsigned short *pus = &us;
    int *pi = &i;
    unsigned int *pui = &ui;
    long int *pli = &li;
    unsigned long int *puli = &uli;
    long long int *plli = &lli;
    unsigned long long int *pulli = &ulli;
    float *pf = &f;
    double *pd = &d;
    long double *pld = &ld;

    printf("Avant la manipulation :\n");
    afficher_variable("char", pc, sizeof(*pc));
    afficher_variable("signed char", psc, sizeof(*psc));
    afficher_variable("unsigned char", puc, sizeof(*puc));
    afficher_variable("short", ps, sizeof(*ps));
    afficher_variable("unsigned short", pus, sizeof(*pus));
    afficher_variable("int", pi, sizeof(*pi));
    afficher_variable("unsigned int", pui, sizeof(*pui));
    afficher_variable("long int", pli, sizeof(*pli));
    afficher_variable("unsigned long int", puli, sizeof(*puli));
    afficher_variable("long long int", plli, sizeof(*plli));
    afficher_variable("unsigned long long int", pulli, sizeof(*pulli));
    afficher_variable("float", pf, sizeof(*pf));
    afficher_variable("double", pd, sizeof(*pd));
    afficher_variable("long double", pld, sizeof(*pld));

    *pc += 1;
    *psc += 1;
    *puc += 1;
    *ps += 1;
    *pus += 1;
    *pi += 1;
    *pui += 1U;
    *pli += 1L;
    *puli += 1UL;
    *plli += 1LL;
    *pulli += 1ULL;
    *pf += 1.0f;
    *pd += 1.0;
    *pld += 1.0L;

    printf("\nApres la manipulation :\n");
    afficher_variable("char", pc, sizeof(*pc));
    afficher_variable("signed char", psc, sizeof(*psc));
    afficher_variable("unsigned char", puc, sizeof(*puc));
    afficher_variable("short", ps, sizeof(*ps));
    afficher_variable("unsigned short", pus, sizeof(*pus));
    afficher_variable("int", pi, sizeof(*pi));
    afficher_variable("unsigned int", pui, sizeof(*pui));
    afficher_variable("long int", pli, sizeof(*pli));
    afficher_variable("unsigned long int", puli, sizeof(*puli));
    afficher_variable("long long int", plli, sizeof(*plli));
    afficher_variable("unsigned long long int", pulli, sizeof(*pulli));
    afficher_variable("float", pf, sizeof(*pf));
    afficher_variable("double", pd, sizeof(*pd));
    afficher_variable("long double", pld, sizeof(*pld));

    return 0;
}