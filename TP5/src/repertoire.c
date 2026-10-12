#include "repertoire.h"

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

static char *joindre_chemin(const char *repertoire, const char *nom)
{
    size_t longueur_repertoire = strlen(repertoire);
    size_t longueur_nom = strlen(nom);
    int separateur = longueur_repertoire > 0
        && repertoire[longueur_repertoire - 1] != '/';
    char *chemin = malloc(longueur_repertoire + (size_t)separateur
                          + longueur_nom + 1);
    if (chemin == NULL)
        return NULL;

    memcpy(chemin, repertoire, longueur_repertoire);
    if (separateur)
        chemin[longueur_repertoire++] = '/';
    memcpy(chemin + longueur_repertoire, nom, longueur_nom + 1);
    return chemin;
}

int lire_dossier(const char *nom_repertoire)
{
    DIR *dossier = opendir(nom_repertoire);
    if (dossier == NULL)
    {
        perror(nom_repertoire);
        return 0;
    }

    int succes = 1;
    struct dirent *entree;
    for (;;)
    {
        errno = 0;
        entree = readdir(dossier);
        if (entree == NULL)
            break;
        printf("%s\n", entree->d_name);
    }
    if (errno != 0)
    {
        perror("readdir");
        succes = 0;
    }

    if (closedir(dossier) != 0)
    {
        perror("closedir");
        succes = 0;
    }
    return succes;
}

static int parcourir_recursivement(const char *repertoire)
{
    DIR *dossier = opendir(repertoire);
    if (dossier == NULL)
    {
        perror(repertoire);
        return 0;
    }

    int succes = 1;
    struct dirent *entree;
    for (;;)
    {
        errno = 0;
        entree = readdir(dossier);
        if (entree == NULL)
        {
            if (errno != 0)
            {
                perror("readdir");
                succes = 0;
            }
            break;
        }
        if (strcmp(entree->d_name, ".") == 0 || strcmp(entree->d_name, "..") == 0)
            continue;

        char *chemin = joindre_chemin(repertoire, entree->d_name);
        if (chemin == NULL)
        {
            perror("malloc");
            succes = 0;
            continue;
        }

        printf("%s\n", chemin);
        struct stat informations;
        if (lstat(chemin, &informations) != 0)
        {
            perror(chemin);
            succes = 0;
        }
        else if (S_ISDIR(informations.st_mode)
                 && !parcourir_recursivement(chemin))
        {
            succes = 0;
        }
        free(chemin);
    }

    if (closedir(dossier) != 0)
    {
        perror("closedir");
        succes = 0;
    }
    return succes;
}

int lire_dossier_recursif(const char *nom_repertoire)
{
    return parcourir_recursivement(nom_repertoire);
}

int lire_dossier_iteratif(const char *nom_repertoire)
{
    size_t capacite = 16;
    size_t nombre = 1;
    char **pile = malloc(capacite * sizeof(*pile));
    if (pile == NULL)
    {
        perror("malloc");
        return 0;
    }

    pile[0] = malloc(strlen(nom_repertoire) + 1);
    if (pile[0] == NULL)
    {
        perror("malloc");
        free(pile);
        return 0;
    }
    strcpy(pile[0], nom_repertoire);

    int succes = 1;
    while (nombre > 0)
    {
        char *repertoire = pile[--nombre];
        DIR *dossier = opendir(repertoire);
        if (dossier == NULL)
        {
            perror(repertoire);
            succes = 0;
            free(repertoire);
            continue;
        }

        struct dirent *entree;
        for (;;)
        {
            errno = 0;
            entree = readdir(dossier);
            if (entree == NULL)
            {
                if (errno != 0)
                {
                    perror("readdir");
                    succes = 0;
                }
                break;
            }
            if (strcmp(entree->d_name, ".") == 0
                || strcmp(entree->d_name, "..") == 0)
                continue;

            char *chemin = joindre_chemin(repertoire, entree->d_name);
            if (chemin == NULL)
            {
                perror("malloc");
                succes = 0;
                continue;
            }
            printf("%s\n", chemin);

            struct stat informations;
            if (lstat(chemin, &informations) != 0)
            {
                perror(chemin);
                succes = 0;
                free(chemin);
                continue;
            }
            if (S_ISDIR(informations.st_mode))
            {
                if (nombre == capacite)
                {
                    size_t nouvelle_capacite = capacite * 2;
                    char **nouvelle_pile = realloc(
                        pile, nouvelle_capacite * sizeof(*pile));
                    if (nouvelle_pile == NULL)
                    {
                        perror("realloc");
                        succes = 0;
                        free(chemin);
                        continue;
                    }
                    pile = nouvelle_pile;
                    capacite = nouvelle_capacite;
                }
                pile[nombre++] = chemin;
            }
            else
                free(chemin);
        }

        if (closedir(dossier) != 0)
        {
            perror("closedir");
            succes = 0;
        }
        free(repertoire);
    }

    free(pile);
    return succes;
}