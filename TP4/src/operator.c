#include "operator.h"

#include <limits.h>

int somme(int num1, int num2)
{
    return num1 + num2;
}

int difference(int num1, int num2)
{
    return num1 - num2;
}

int produit(int num1, int num2)
{
    return num1 * num2;
}

int quotient(int num1, int num2)
{
    return num1 / num2;
}

int modulo(int num1, int num2)
{
    return num1 % num2;
}

int et(int num1, int num2)
{
    return num1 & num2;
}

int ou(int num1, int num2)
{
    return num1 | num2;
}

int negation(int num1, int num2)
{
    (void)num2;
    return ~num1;
}

int calculer(int num1, int num2, char op, int *resultat)
{
    if (resultat == 0)
        return 0;

    long long valeur;
    switch (op)
    {
        case '+':
            valeur = (long long)num1 + num2;
            if (valeur < INT_MIN || valeur > INT_MAX)
                return 0;
            *resultat = somme(num1, num2);
            break;
        case '-':
            valeur = (long long)num1 - num2;
            if (valeur < INT_MIN || valeur > INT_MAX)
                return 0;
            *resultat = difference(num1, num2);
            break;
        case '*':
            valeur = (long long)num1 * num2;
            if (valeur < INT_MIN || valeur > INT_MAX)
                return 0;
            *resultat = produit(num1, num2);
            break;
        case '/':
            if (num2 == 0 || (num1 == INT_MIN && num2 == -1))
                return 0;
            *resultat = quotient(num1, num2);
            break;
        case '%':
            if (num2 == 0)
                return 0;
            if (num1 == INT_MIN && num2 == -1)
            {
                *resultat = 0;
                break;
            }
            *resultat = modulo(num1, num2);
            break;
        case '&':
            *resultat = et(num1, num2);
            break;
        case '|':
            *resultat = ou(num1, num2);
            break;
        case '~':
            *resultat = negation(num1, num2);
            break;
        default:
            return 0;
    }

    return 1;
}