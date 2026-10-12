/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef __CLIENT_H__
#define __CLIENT_H__

/* Port TCP utilisé pour échanger des requêtes JSON délimitées par une ligne. */
#define PORT 8089

/* Envoie une requête JSON message ou calcul et affiche la réponse JSON. */
int envoie_recois_message(int socketfd);

#endif
