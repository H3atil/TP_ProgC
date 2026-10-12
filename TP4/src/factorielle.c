#include <stdio.h>

static int factorielle(int num)
{
    if (num == 0)
        return 1;
    return num * factorielle(num - 1);
}

int main(void)
{
    const int valeurs[] = {0, 1, 5, 10, 12};

    for (size_t i = 0; i < sizeof(valeurs) / sizeof(valeurs[0]); i++)
        printf("fact(%d): %d\n", valeurs[i], factorielle(valeurs[i]));

    return 0;
}