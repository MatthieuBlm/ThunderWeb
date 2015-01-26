#include "socket.h"

int creer_serveur(int port){
	// ---------------------------------------- INITIALISATION SERVEUR ----------------------------------------
	// Initialisation des signaux systèmes
	initialiser_signaux();

	// Initialisation de la socket serveur
	int socket_serveur = creer_socket_serveur();

	// Liaison entre l'interface et la socket
	liaison_interface_socket(port, socket_serveur);

	// Paramétrage pour réutiliser l'interface directement après l'extinction du serveur
	int optval = 1;
	if(setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1){
		perror("Impossible de mettre le paramètre SO_REUSEADDR");
		exit(1);
	}

	// ---------------------------------------- INITIALISATION CLIENT ----------------------------------------
	
	while(1){// Initialisation socket client
		int socket_client = creer_socket_client(socket_serveur);
		int fd_client_fork = fork();
		if(fd_client_fork == -1){
			perror("fork");
			exit(1);
		} else if(fd_client_fork == 0) {
			// Dans le processus fils

			// Envoi du message de bienvenue
			const char * message_bienvenue = "----- THUNDERWEB -----\nBienvenue sur notre serveur Web.\nCeci est notre message de bienvenue !\nBonne visite ;)\n" ;
			sleep(1);
			write(socket_client, message_bienvenue, strlen(message_bienvenue));

			// Séquence de renvoi des informations du client vers le client
			traitement_client(socket_client);
		}
	}

	return socket_serveur;
}

int creer_socket_serveur(){
	int socket_serveur;
	socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_serveur == -1) {
		perror("socket_serveur");
		exit(1);
	}
	return socket_serveur;
}

int creer_socket_client(int socket_serveur){
	int socket_client;
	socket_client = accept(socket_serveur, NULL, NULL);
	if (socket_client == -1) {
		perror("accept");
		exit(1);
	}
	return socket_client;
}

void initialiser_signaux(){
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR){
		perror("signal");
	}
}

void traitement_client(int socket_client){
	char message[256];
	int message_length;
	while(1){
		message_length = read(socket_client, &message, sizeof(message));
		if(message_length == -1){
			perror("read");
			exit(1);
		} else {
			write(socket_client, message, message_length);
		}
	}
}

void liaison_interface_socket(int port, int socket_serveur){
	struct sockaddr_in saddr;
	saddr.sin_family = AF_INET; /* Socket ipv4 */
	saddr.sin_port = htons(port); /* Port d'écoute */
	saddr.sin_addr.s_addr = INADDR_ANY; /* écoute sur toutes les interfaces */
	if(bind(socket_serveur, (struct sockaddr *)&saddr, sizeof(saddr)) == -1) {
		perror("bind socket_serveur");
		exit(1);
	}

	if (listen(socket_serveur, 10) == -1){
		perror("listen socket_serveur");
		exit(1);
	}
}