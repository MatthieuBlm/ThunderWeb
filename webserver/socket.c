#include "socket.h"

int creer_serveur(int port){

	// Initialisation de la socket

	int fd_socket;
	fd_socket = socket(AF_INET, SOCK_STREAM, 0);
	if(fd_socket == -1) {
		perror("fd_socket");
		return -1;
	}

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET; /* Socket ipv4 */
	saddr.sin_port = htons(port); /* Port d'écoute */
	saddr.sin_addr.s_addr = INADDR_ANY; /* écoute sur toutes les interfaces */
	if(bind(fd_socket, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		perror("bind fd_socket");
		return -1;
	}

	if (listen(fd_socket, 10) == -1){
		perror("listen fd_socket");
		return -1;
	}

	printf("Test\n");

	return fd_socket;
}