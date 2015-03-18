#include "socket.h"
int main ()
{

	printf("[Info] Démarrage du serveur\n");
	int socket_serveur = creer_serveur(8080);

	printf("[OK] Serveur démarré\n");

	printf("[Info] Initialisation des statistiques\n");
	init_stats();
	printf("[OK] Statistiques initialisées\n--------------------\n");

	while(1){
		// Initialisation socket client
		int socket_client = creer_socket_client(socket_serveur);
		int fd_client_fork = fork();
		if(fd_client_fork == -1){
			perror("fork");
			exit(1);
		} else if(fd_client_fork == 0) {
			// Dans le processus fils
			sleep(1);
			(get_stats()->served_connections)++;

			// Séquence de renvoi des informations du client vers le client
			traitement_client(socket_client);
			exit(0);
		} else {
			close(socket_client);
		}
	}
	return 0;
}

