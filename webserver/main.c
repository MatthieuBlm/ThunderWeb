#include "socket.h"
int main ()
{
	int socket_serveur = creer_serveur(8080);

	// Paramétrage pour réutiliser l'interface directement après l'extinction du serveur
	int optval = 1;
	if(setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1){
		perror("Impossible de mettre le paramètre SO_REUSEADDR");
		exit(1);
	}

	while(1){
		// Initialisation socket client
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
			exit(0);
		} else {
			close(socket_client);
		}
	}
	return 0;
}

