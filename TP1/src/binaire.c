#include <limits.h>
#include <stdio.h>

int main(void)
{
    const unsigned int nombres[] = {0, 4096, 65536, 65535, 1024};
    const size_t nombre_total = sizeof(nombres) / sizeof(nombres[0]);
    int chiffres[sizeof(unsigned int) * CHAR_BIT];

    for (size_t i = 0; i < nombre_total; i++)
    {
        unsigned int nombre = nombres[i];
        size_t longueur = 0;

        do
        {
            chiffres[longueur] = nombre % 2;
            nombre /= 2;
            longueur++;
        } while (nombre != 0);

        printf("%u en binaire : ", nombres[i]);
        for (size_t j = longueur; j > 0; j--)
            printf("%d", chiffres[j - 1]);
        printf("\n");
    }

    return 0;
}