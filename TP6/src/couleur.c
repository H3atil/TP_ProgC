/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#include "couleur.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static uint32_t couleur_code24(couleur24 c)
{
  return ((uint32_t)c.rouge << 16)
       | ((uint32_t)c.vert << 8)
       | (uint32_t)c.bleu;
}

static uint32_t couleur_code32(couleur32 c)
{
  return ((uint32_t)c.rouge << 24)
       | ((uint32_t)c.vert << 16)
       | ((uint32_t)c.bleu << 8)
       | (uint32_t)c.alpha;
}

static size_t hacher(uint32_t valeur)
{
  valeur ^= valeur >> 16;
  valeur *= UINT32_C(0x7feb352d);
  valeur ^= valeur >> 15;
  valeur *= UINT32_C(0x846ca68b);
  valeur ^= valeur >> 16;
  return (size_t)valeur;
}

couleur_compteur *compte_couleur(couleur *c, int csize)
{
  if (c == NULL || csize <= 0
      || (c->compte_bit != BITS24 && c->compte_bit != BITS32))
    return NULL;

  size_t capacite = 2;
  while (capacite < (size_t)csize * 2)
  {
    if (capacite > SIZE_MAX / 2)
      return NULL;
    capacite *= 2;
  }

  size_t *cases = calloc(capacite, sizeof(*cases));
  couleur_compteur *compteur = calloc(1, sizeof(*compteur));
  if (cases == NULL || compteur == NULL)
  {
    perror("Allocation de mémoire");
    free(cases);
    free(compteur);
    return NULL;
  }

  compteur->compte_bit = c->compte_bit;
  if (c->compte_bit == BITS24)
    compteur->cc.cc24 = calloc((size_t)csize, sizeof(*compteur->cc.cc24));
  else
    compteur->cc.cc32 = calloc((size_t)csize, sizeof(*compteur->cc.cc32));
  if ((c->compte_bit == BITS24 && compteur->cc.cc24 == NULL)
      || (c->compte_bit == BITS32 && compteur->cc.cc32 == NULL))
  {
    perror("Allocation des couleurs");
    free(cases);
    free(compteur->cc.cc24);
    free(compteur);
    return NULL;
  }

  int nombre = 0;
  for (int i = 0; i < csize; i++)
  {
    uint32_t code = c->compte_bit == BITS24
      ? couleur_code24(c->c.c24[i]) : couleur_code32(c->c.c32[i]);
    size_t emplacement = hacher(code) & (capacite - 1);
    while (cases[emplacement] != 0)
    {
      size_t index = cases[emplacement] - 1;
      uint32_t code_existant = c->compte_bit == BITS24
        ? couleur_code24(compteur->cc.cc24[index].c)
        : couleur_code32(compteur->cc.cc32[index].c);
      if (code_existant == code)
        break;
      emplacement = (emplacement + 1) & (capacite - 1);
    }

    if (cases[emplacement] == 0)
    {
      cases[emplacement] = (size_t)nombre + 1;
      if (c->compte_bit == BITS24)
      {
        compteur->cc.cc24[nombre].c = c->c.c24[i];
        compteur->cc.cc24[nombre].compte = 1;
      }
      else
      {
        compteur->cc.cc32[nombre].c = c->c.c32[i];
        compteur->cc.cc32[nombre].compte = 1;
      }
      nombre++;
    }
    else
    {
      size_t index = cases[emplacement] - 1;
      if (c->compte_bit == BITS24)
        compteur->cc.cc24[index].compte++;
      else
        compteur->cc.cc32[index].compte++;
    }
  }

  free(cases);
  compteur->size = nombre;
  return compteur;
}

static int comparer24(const void *gauche, const void *droite)
{
  const couleur24_compteur *a = gauche;
  const couleur24_compteur *b = droite;
  if (a->compte != b->compte)
    return a->compte < b->compte ? 1 : -1;
  uint32_t code_a = couleur_code24(a->c);
  uint32_t code_b = couleur_code24(b->c);
  return (code_a > code_b) - (code_a < code_b);
}

static int comparer32(const void *gauche, const void *droite)
{
  const couleur32_compteur *a = gauche;
  const couleur32_compteur *b = droite;
  if (a->compte != b->compte)
    return a->compte < b->compte ? 1 : -1;
  uint32_t code_a = couleur_code32(a->c);
  uint32_t code_b = couleur_code32(b->c);
  return (code_a > code_b) - (code_a < code_b);
}

void print_couleur(couleur *c, int csize)
{
  if (c == NULL || csize < 0)
    return;
  for (int i = 0; i < csize; i++)
  {
    if (c->compte_bit == BITS24)
    {
      printf("%02x %02x %02x\n", c->c.c24[i].rouge,
             c->c.c24[i].vert, c->c.c24[i].bleu);
    }
    else if (c->compte_bit == BITS32)
    {
      printf("%02x %02x %02x %02x\n", c->c.c32[i].rouge,
             c->c.c32[i].vert, c->c.c32[i].bleu, c->c.c32[i].alpha);
    }
  }
}

void print_couleur_compteur(couleur_compteur *ccompteur)
{
  if (ccompteur == NULL)
    return;
  for (int i = 0; i < ccompteur->size; i++)
  {
    if (ccompteur->compte_bit == BITS24)
    {
      couleur24 c = ccompteur->cc.cc24[i].c;
      printf("%02x %02x %02x: %d\n", c.rouge, c.vert,
             c.bleu, ccompteur->cc.cc24[i].compte);
    }
    else if (ccompteur->compte_bit == BITS32)
    {
      couleur32 c = ccompteur->cc.cc32[i].c;
      printf("%02x %02x %02x %02x: %d\n", c.rouge, c.vert,
             c.bleu, c.alpha, ccompteur->cc.cc32[i].compte);
    }
  }
}

void trier_couleur_compteur(couleur_compteur *ccompteur)
{
  if (ccompteur == NULL || ccompteur->size < 2)
    return;
  if (ccompteur->compte_bit == BITS24)
    qsort(ccompteur->cc.cc24, (size_t)ccompteur->size,
          sizeof(*ccompteur->cc.cc24), comparer24);
  else if (ccompteur->compte_bit == BITS32)
    qsort(ccompteur->cc.cc32, (size_t)ccompteur->size,
          sizeof(*ccompteur->cc.cc32), comparer32);
}

void liberer_couleur_compteur(couleur_compteur *ccompteur)
{
  if (ccompteur == NULL)
    return;
  if (ccompteur->compte_bit == BITS24)
    free(ccompteur->cc.cc24);
  else if (ccompteur->compte_bit == BITS32)
    free(ccompteur->cc.cc32);
  free(ccompteur);
}
