#include "socket.h"

int creer_serveur(int port){
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

	return socket_serveur;
}

int creer_socket_serveur(void){
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

void initialiser_signaux(void){
	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR){
		perror("signal");
		exit(1);
	}

	struct sigaction sa;
	sa.sa_handler = traitement_signal;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;

	if(sigaction(SIGCHLD, &sa, NULL) == -1) {
		perror("sigaction(SIGCHLD)");
		exit(1);
	}
}

void traitement_signal(int sig){
	if(sig == SIGCHLD){
		waitpid(-1, 0, WNOHANG);
	}
}


void traitement_client(int socket_client){
	/*
	
	----------------------------------- ANCIENNE VERSION (sans fgets) -----------------------------------

	char message[256];
	int message_length;
	while((message_length = read(socket_client, &message, sizeof(message))) != 0){
		if(message_length == -1){
			perror("read");
			exit(1);  
		} else {
			write(socket_client, message, message_length);
		}
	}

	*/

	FILE * f = fdopen(socket_client, "w+");
	char * message = malloc(8192);
	if(f == NULL){
		perror("fdopen");
		exit(1);
	}

	char * res;
	

	while((res = fgets(message, 8192, f)) != NULL){
		fprintf(f, "<ThunderWeb> %s\n", message);
	}

	free(message);
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