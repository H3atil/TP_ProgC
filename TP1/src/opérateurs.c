#include <stdio.h>

int main(void)
{
    int a = 16;
    int b = 3;

    printf("Addition : %d\n", a + b);
    printf("Soustraction : %d\n", a - b);
    printf("Multiplication : %d\n", a * b);
    printf("Division : %d\n", a / b);
    printf("Modulo : %d\n", a % b);
    printf("a == b : %s\n", a == b ? "true" : "false");
    printf("a > b : %s\n", a > b ? "true" : "false");

    return 0;
}