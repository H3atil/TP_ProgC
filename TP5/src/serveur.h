/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

#ifndef __SERVER_H__
#define __SERVER_H__

#define PORT 8089

int renvoie_message(int socketfd, const char *message);
int recois_numeros_calcule(int socketfd, const char *message);

#endif
