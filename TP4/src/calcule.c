#include "operator.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

static int convertir_entier(const char *texte, int *valeur)
{
    char *fin;
    errno = 0;
    long resultat = strtol(texte, &fin, 10);
    if (errno != 0 || fin == texte || *fin != '\0'
        || resultat < INT_MIN || resultat > INT_MAX)
        return 0;
    *valeur = (int)resultat;
    return 1;
}

int main(int argc, char **argv)
{
    int num1;
    int num2;
    int resultat;

    if (argc != 4 || argv[1][0] == '\0' || argv[1][1] != '\0'
        || !convertir_entier(argv[2], &num1)
        || !convertir_entier(argv[3], &num2))
    {
        fprintf(stderr, "Usage : %s <opérateur> <num1> <num2>\n", argv[0]);
        return 1;
    }

    if (!calculer(num1, num2, argv[1][0], &resultat))
    {
        fprintf(stderr, "Operateur invalide ou division par zero.\n");
        return 1;
    }
    printf("Résultat : %d\n", resultat);
    return 0;
}