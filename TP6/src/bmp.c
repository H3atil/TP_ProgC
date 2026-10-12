#include "bmp.h"

#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int lire_exactement(int fd, void *tampon, size_t taille)
{
    unsigned char *octets = tampon;
    size_t lus = 0;

    while (lus < taille)
    {
        ssize_t resultat = read(fd, octets + lus, taille - lus);
        if (resultat < 0 && errno == EINTR)
            continue;
        if (resultat <= 0)
            return 0;
        lus += (size_t)resultat;
    }
    return 1;
}

couleur_compteur *analyse_bmp_image(char *nom_de_fichier)
{
    int fd = open(nom_de_fichier, O_RDONLY);
    if (fd < 0)
    {
        perror(nom_de_fichier);
        return NULL;
    }

    couleur_compteur *resultat = NULL;
    bmp_header entete;
    bmp_info_header informations;
    struct stat statut_fichier;
    unsigned char *ligne = NULL;
    couleur24 *pixels24 = NULL;
    couleur32 *pixels32 = NULL;

    if (!lire_exactement(fd, &entete, sizeof(entete))
        || entete.type != 0x4d42
        || !lire_exactement(fd, &informations, sizeof(informations)))
    {
        fprintf(stderr, "%s : en-tête BMP invalide ou tronqué.\n", nom_de_fichier);
        goto fin;
    }
    if (informations.info_header_size < sizeof(informations)
        || informations.largeur <= 0 || informations.hauteur == 0
        || informations.planes != 1 || informations.compression != 0
        || (informations.compte_bit != 24 && informations.compte_bit != 32)
        || (uint64_t)entete.offset
           < sizeof(entete) + (uint64_t)informations.info_header_size)
    {
        fprintf(stderr, "%s : format BMP non pris en charge.\n", nom_de_fichier);
        goto fin;
    }
    if (fstat(fd, &statut_fichier) != 0)
    {
        perror("fstat");
        goto fin;
    }

    uint64_t hauteur = informations.hauteur < 0
        ? (uint64_t)(-(int64_t)informations.hauteur)
        : (uint64_t)informations.hauteur;
    uint64_t largeur = (uint64_t)informations.largeur;
    uint64_t nombre_pixels = largeur * hauteur;
    uint64_t bits_par_ligne = largeur * informations.compte_bit;
    uint64_t taille_ligne = ((bits_par_ligne + 31) / 32) * 4;
    if (taille_ligne > UINT64_MAX / hauteur)
    {
        fprintf(stderr, "%s : dimensions BMP invalides.\n", nom_de_fichier);
        goto fin;
    }
    uint64_t taille_pixels = taille_ligne * hauteur;
    if (taille_pixels > UINT64_MAX - entete.offset)
    {
        fprintf(stderr, "%s : dimensions BMP invalides.\n", nom_de_fichier);
        goto fin;
    }
    uint64_t fin_image = (uint64_t)entete.offset + taille_pixels;

    if (nombre_pixels == 0 || nombre_pixels > INT_MAX
        || taille_ligne > SIZE_MAX
        || nombre_pixels > SIZE_MAX / sizeof(couleur32)
        || fin_image > (uint64_t)statut_fichier.st_size)
    {
        fprintf(stderr, "%s : dimensions BMP invalides ou fichier tronqué.\n",
                nom_de_fichier);
        goto fin;
    }

    ligne = malloc((size_t)taille_ligne);
    if (ligne == NULL)
    {
        perror("malloc");
        goto fin;
    }
    if (lseek(fd, (off_t)entete.offset, SEEK_SET) != (off_t)entete.offset)
    {
        perror("lseek");
        goto fin;
    }

    couleur pixels;
    if (informations.compte_bit == 24)
    {
        pixels24 = malloc((size_t)nombre_pixels * sizeof(*pixels24));
        if (pixels24 == NULL)
        {
            perror("malloc");
            goto fin;
        }
        pixels.compte_bit = BITS24;
        pixels.c.c24 = pixels24;
    }
    else
    {
        pixels32 = malloc((size_t)nombre_pixels * sizeof(*pixels32));
        if (pixels32 == NULL)
        {
            perror("malloc");
            goto fin;
        }
        pixels.compte_bit = BITS32;
        pixels.c.c32 = pixels32;
    }

    size_t index = 0;
    size_t octets_par_pixel = informations.compte_bit / 8;
    for (uint64_t y = 0; y < hauteur; y++)
    {
        if (!lire_exactement(fd, ligne, (size_t)taille_ligne))
        {
            fprintf(stderr, "%s : données BMP tronquées.\n", nom_de_fichier);
            goto fin;
        }
        for (uint64_t x = 0; x < largeur; x++)
        {
            const unsigned char *pixel = ligne + x * octets_par_pixel;
            if (informations.compte_bit == 24)
            {
                pixels24[index].bleu = pixel[0];
                pixels24[index].vert = pixel[1];
                pixels24[index].rouge = pixel[2];
            }
            else
            {
                pixels32[index].bleu = pixel[0];
                pixels32[index].vert = pixel[1];
                pixels32[index].rouge = pixel[2];
                pixels32[index].alpha = pixel[3];
            }
            index++;
        }
    }

    resultat = compte_couleur(&pixels, (int)nombre_pixels);
    if (resultat == NULL)
        fprintf(stderr, "Impossible de compter les couleurs de %s.\n", nom_de_fichier);
    else
        trier_couleur_compteur(resultat);

fin:
    free(ligne);
    free(pixels24);
    free(pixels32);
    if (close(fd) != 0)
    {
        perror("close");
        liberer_couleur_compteur(resultat);
        return NULL;
    }
    return resultat;
}
