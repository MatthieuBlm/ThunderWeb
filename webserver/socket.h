#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/mman.h>

#include "stats.h"

#ifndef __SOCKET_H__
#define __SOCKET_H__

enum http_method {
	HTTP_GET ,
	HTTP_UNSUPPORTED ,
};

typedef struct
{
	enum http_method method ;
	int major_version ;
	int minor_version ;
	char * url ;
} http_request ;

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

//char * traitement_first_line(const char * req);
int parse_http_request ( const char * request_line , http_request * request );

char** split(const char* chaine, char* delim,int vide);

void initialiser_signaux(void);
void traitement_signal(int sig);

char * fgets_or_exit(char * buffer, int size, FILE *stream);
void skip_headers(FILE *client);
void send_status(FILE * client , int code , const char * reason_phrase);
void send_response(FILE * client , int code , const char * reason_phrase, const char * message_body);
void send_file(FILE * client , int code , const char * reason_phrase, int fdFile);
void send_stats ( FILE * client );

char * rewrite_url ( char * url );
int check_and_open ( const char * url , const char * document_root );
int get_file_size(int fd);
int copy(int in, int out);

#endif
