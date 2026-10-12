#include <stdio.h>

struct Couleur
{
    unsigned char rouge;
    unsigned char vert;
    unsigned char bleu;
    unsigned char alpha;
};

struct CouleurComptee
{
    struct Couleur couleur;
    int occurrences;
};

static int memes_couleurs(struct Couleur gauche, struct Couleur droite)
{
    return gauche.rouge == droite.rouge
        && gauche.vert == droite.vert
        && gauche.bleu == droite.bleu
        && gauche.alpha == droite.alpha;
}

int main(void)
{
    const struct Couleur palette[] = {
        {0xff, 0x23, 0x23, 0x45},
        {0xff, 0x00, 0x23, 0x12},
        {0x00, 0x80, 0xff, 0xff},
        {0x11, 0x22, 0x33, 0x44},
        {0xaa, 0xbb, 0xcc, 0xdd}
    };
    struct Couleur couleurs[100];
    struct CouleurComptee distinctes[100];
    int nombre_distinct = 0;

    for (int i = 0; i < 100; i++)
        couleurs[i] = palette[i % (int)(sizeof(palette) / sizeof(palette[0]))];

    for (int i = 0; i < 100; i++)
    {
        int j;
        for (j = 0; j < nombre_distinct; j++)
        {
            if (memes_couleurs(couleurs[i], distinctes[j].couleur))
            {
                distinctes[j].occurrences++;
                break;
            }
        }

        if (j == nombre_distinct)
        {
            distinctes[nombre_distinct].couleur = couleurs[i];
            distinctes[nombre_distinct].occurrences = 1;
            nombre_distinct++;
        }
    }

    for (int i = 0; i < nombre_distinct; i++)
    {
        struct Couleur couleur = distinctes[i].couleur;
        printf("0x%02x 0x%02x 0x%02x 0x%02x : %d\n",
               (unsigned int)couleur.rouge,
               (unsigned int)couleur.vert,
               (unsigned int)couleur.bleu,
               (unsigned int)couleur.alpha,
               distinctes[i].occurrences);
    }

    return 0;
}