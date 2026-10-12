#include "liste.h"

#include <stdio.h>
#include <stdlib.h>

struct noeud_couleur
{
    struct couleur valeur;
    struct noeud_couleur *suivant;
};

void init_liste(struct liste_couleurs *liste)
{
    liste->tete = NULL;
}

int insertion(const struct couleur *couleur, struct liste_couleurs *liste)
{
    struct noeud_couleur *noeud = malloc(sizeof(*noeud));
    if (noeud == NULL)
        return 0;

    noeud->valeur = *couleur;
    noeud->suivant = NULL;
    struct noeud_couleur **lien = &liste->tete;
    while (*lien != NULL)
        lien = &(*lien)->suivant;
    *lien = noeud;
    return 1;
}

void parcours(const struct liste_couleurs *liste)
{
    const struct noeud_couleur *courant = liste->tete;
    while (courant != NULL)
    {
        printf("0x%02x 0x%02x 0x%02x 0x%02x\n",
               (unsigned int)courant->valeur.rouge,
               (unsigned int)courant->valeur.vert,
               (unsigned int)courant->valeur.bleu,
               (unsigned int)courant->valeur.alpha);
        courant = courant->suivant;
    }
}

void detruire_liste(struct liste_couleurs *liste)
{
    struct noeud_couleur *courant = liste->tete;
    while (courant != NULL)
    {
        struct noeud_couleur *suivant = courant->suivant;
        free(courant);
        courant = suivant;
    }
    liste->tete = NULL;
}