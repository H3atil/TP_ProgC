/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef __SERVER_H__
#define __SERVER_H__

/* Le serveur reçoit et renvoie des objets JSON délimités par une ligne. */
#define PORT 8089

extern const char *svg_file_path;

/* Analyse un objet JSON et traite une opération message, calcule ou couleurs. */
int recois_envoie_message(int socketfd, const char *message);

#endif
