#include <stdio.h>

int main(void)
{
    int num1;
    int num2;
    char op;

    printf("Entrez num1, num2 et l'operateur (+, -, *, /, %%, &, |, ~) : ");
    if (scanf("%d %d %c", &num1, &num2, &op) != 3)
    {
        fprintf(stderr, "Entree invalide\n");
        return 1;
    }

    switch (op)
    {
        case '+':
            printf("Resultat : %d\n", num1 + num2);
            break;

        case '-':
            printf("Resultat : %d\n", num1 - num2);
            break;

        case '*':
            printf("Resultat : %d\n", num1 * num2);
            break;

        case '/':
            if (num2 != 0)
                printf("Resultat : %d\n", num1 / num2);
            else
                printf("Division par zero impossible\n");
            break;

        case '%':
            if (num2 != 0)
                printf("Resultat : %d\n", num1 % num2);
            else
                printf("Modulo par zero impossible\n");
            break;

        case '&':
            printf("Resultat : %d\n", num1 & num2);
            break;

        case '|':
            printf("Resultat : %d\n", num1 | num2);
            break;

        case '~':
            printf("Resultat : %d\n", ~num1);
            break;

        default:
            printf("Operateur inconnu\n");
    }

    return 0;
}
