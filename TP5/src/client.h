/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

#include <stddef.h>

/*
 * port d'ordinateur pour envoyer et recevoir des messages
 */
#define PORT 8089

int envoie_recois_message(int socketfd);
int envoie_operateur_numeros(int socketfd, char operateur,
                             int num1, int num2, int *resultat);
int envoyer_ligne(int socketfd, const char *message);
int recevoir_ligne(int socketfd, char *message, size_t capacite);

#endif
