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
	
	int first_line = 1;
	while(fgets(message, 8192, f) != NULL){
		//fprintf(f, "<ThunderWeb> %s\n", message);
		printf("[Reçu] %s", message);
		if(first_line){
			if(traitement_requete(message) == -1){
				break;
			}
			first_line = 0;
		}
	}

	free(message);
}

int traitement_requete(const char * req){
	char ** tab = split(req, " ", 0);
	int i;
	for(i = 0; tab[i] != NULL; ++i) {
		//printf("%d : %s\n", i, tab[i]);
	}
	if(i != 3){
		printf("[Warning] Requête invalide : nombre de mots invalide\n");
		free(tab);
		return -1;
	}
	if(strncmp(tab[0], "GET", 3) != 0){
		printf("[Warning] Requête invalide : le premier paramètre est différent de GET\n");
		free(tab);
		return -1;
	}
	free(tab);
	return 0;
}

	

char** split(const char * chaine, char* delim,int vide){

    char** tab=NULL;               //tableau de chaine, tableau resultat
    char *ptr;                     //pointeur sur une partie de
    int sizeStr;                   //taille de la chaine à recupérer
    int sizeTab=0;                 //taille du tableau de chaine

    int sizeDelim=strlen(delim);   //taille du delimiteur


    while( (ptr=strstr(chaine, delim))!=NULL ){
    	sizeStr=ptr-chaine;

           //si la chaine trouvé n'est pas vide ou si on accepte les chaine vide                  
    	if(vide==1 || sizeStr!=0){
               //on alloue une case en plus au tableau de chaines
    		sizeTab++;
    		tab= (char**) realloc(tab,sizeof(char*)*sizeTab);

               //on alloue la chaine du tableau
    		tab[sizeTab-1]=(char*) malloc( sizeof(char)*(sizeStr+1) );
    		strncpy(tab[sizeTab-1],chaine,sizeStr);
    		tab[sizeTab-1][sizeStr]='\0';
    	}

           //on decale le pointeur chaine  pour continuer la boucle apres le premier elément traiter
    	ptr=ptr+sizeDelim;
    	chaine=ptr;
    }

    //si la chaine n'est pas vide, on recupere le dernier "morceau"
    if(strlen(chaine)!=0){
    	sizeStr=strlen(chaine);
    	sizeTab++;
    	tab= (char**) realloc(tab,sizeof(char*)*sizeTab);
    	tab[sizeTab-1]=(char*) malloc( sizeof(char)*(sizeStr+1) );
    	strncpy(tab[sizeTab-1],chaine,sizeStr);
    	tab[sizeTab-1][sizeStr]='\0';
    }
    else if(vide==1){ //si on fini sur un delimiteur et si on accepte les mots vides,on ajoute un mot vide
    	sizeTab++;
    	tab= (char**) realloc(tab,sizeof(char*)*sizeTab);
    	tab[sizeTab-1]=(char*) malloc( sizeof(char)*1 );
    	tab[sizeTab-1][0]='\0';

    }

    //on ajoute une case à null pour finir le tableau
    sizeTab++;
    tab= (char**) realloc(tab,sizeof(char*)*sizeTab);
    tab[sizeTab-1]=NULL;

    return tab;
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