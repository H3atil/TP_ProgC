#include <stdio.h>

int main(void)
{
    char c = 'A';
    signed char sc = -100;
    unsigned char uc = 200;

    short s = -32000;
    unsigned short us = 60000;

    int i = -1000;
    unsigned int ui = 1000;

    long int li = -100000;
    unsigned long int uli = 100000;

    long long int lli = -1000000000;
    unsigned long long int ulli = 1000000000;

    float f = 3.14f;
    double d = 3.14159;
    long double ld = 3.1415926535L;

    printf("char : %c\n", c);
    printf("signed char : %hhd\n", sc);
    printf("unsigned char : %hhu\n", uc);

    printf("short : %hd\n", s);
    printf("unsigned short : %hu\n", us);

    printf("int : %d\n", i);
    printf("unsigned int : %u\n", ui);

    printf("long int : %ld\n", li);
    printf("unsigned long int : %lu\n", uli);

    printf("long long int : %lld\n", lli);
    printf("unsigned long long int : %llu\n", ulli);

    printf("float : %f\n", f);
    printf("double : %f\n", d);
    printf("long double : %Lf\n", ld);

    return 0;
}