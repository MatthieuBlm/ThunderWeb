#include "socket.h"

int creer_serveur(int port){

	// Initialisation de la socket serveur

	int socket_serveur;
	socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_serveur == -1) {
		perror("socket_serveur");
		return -1;
	}

	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET; /* Socket ipv4 */
	saddr.sin_port = htons(port); /* Port d'écoute */
	saddr.sin_addr.s_addr = INADDR_ANY; /* écoute sur toutes les interfaces */
	if(bind(socket_serveur, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		perror("bind socket_serveur");
		return -1;
	}

	if (listen(socket_serveur, 10) == -1){
		perror("listen socket_serveur");
		return -1;
	}

	// Initialisation socket client

	int socket_client;
	socket_client = accept(socket_serveur, NULL, NULL);
	if (socket_client == -1) {
		perror("accept");
	}

	// Envoi du message de bienvenue

	const char * message_bienvenue = "----- THUNDERWEB -----\nBienvenue sur notre serveur Web.\nCeci est notre message de bienvenue !\nBonne visite ;)\n" ;
	sleep(1);
	write(socket_client, message_bienvenue, strlen(message_bienvenue));

	// Séquence de renvoi des informations du client vers le client
	char message[256];
	int message_length;
	while(1){
		message_length = read(socket_client, &message, sizeof(message));
		if(message_length == -1){
			perror("read");
		} else {
			write(socket_client, message, message_length);
		}
	}

	return socket_serveur;
}