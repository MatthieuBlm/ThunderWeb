#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#ifndef __SOCKET_H__
#define __SOCKET_H__

/** Crée une socket serveur qui écoute sur toute les interfaces IPv4
de la machine sur le port passé en paramètre. La socket retournée
doit pouvoir être utilisée directement par un appel à accept.

La fonction retourne -1 en cas d'erreur ou le descripteur de la
socket créée. */

int creer_serveur(int port);

/** Crée une socket pour le serveur.

La fonction retourne -1 en cas d'erreur ou le descripteur de la
socket créée.
*/

int creer_socket_serveur(void);

/** Crée une socket pour le client.

La fonction retourne -1 en cas d'erreur ou le descripteur de la
socket créée.
*/

int creer_socket_client(int socket_serveur);

void traitement_client(int socket_client);
void liaison_interface_socket(int port, int socket_serveur);

void initialiser_signaux(void);
void traitement_signal(int sig);
#endif
