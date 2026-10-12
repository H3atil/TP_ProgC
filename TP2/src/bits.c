#include <limits.h>
#include <stdio.h>

int main(void)
{
    const int largeur = (int)(sizeof(int) * CHAR_BIT);
    int d = (1U << (largeur - 4)) | (1U << (largeur - 20));
    int quatrieme_bit = (d >> (largeur - 4)) & 1;
    int vingtieme_bit = (d >> (largeur - 20)) & 1;

    printf("%d\n", quatrieme_bit && vingtieme_bit);
    return 0;
}