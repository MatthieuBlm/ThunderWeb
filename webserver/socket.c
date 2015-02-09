#include "socket.h"

int creer_serveur(int port){
	// Initialisation des signaux systèmes
	initialiser_signaux();

	// Initialisation de la socket serveur
	int socket_serveur = creer_socket_serveur();

	printf("[Info] Mise en place du paramétrage SO_REUSEADDR\n");
	// Paramétrage pour réutiliser l'interface directement après l'extinction du serveur
	int optval = 1;
	if(setsockopt(socket_serveur, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int)) == -1){
		perror("Impossible de mettre le paramètre SO_REUSEADDR");
		exit(1);
	}

	// Liaison entre l'interface et la socket
	liaison_interface_socket(port, socket_serveur);	

	return socket_serveur;
}

int creer_socket_serveur(void){
	printf("[Info] Création de la socket serveur\n");
	int socket_serveur;
	socket_serveur = socket(AF_INET, SOCK_STREAM, 0);
	if(socket_serveur == -1) {
		perror("socket_serveur");
		exit(1);
	}
	printf("[Info] Socket serveur créée\n");
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
	printf("[Info] Initialisation des signaux\n");
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
	printf("[Info] Signaux initialisés\n");
}

void traitement_signal(int sig){
	if(sig == SIGCHLD){
		waitpid(-1, 0, WNOHANG);
	}
}


void traitement_client(int socket_client){
	FILE * f = fdopen(socket_client, "w+");
	char * message = malloc(512);
	char * lignesMessage = malloc(8192);

	if(f == NULL){
		perror("fdopen");
		exit(1);
	}

	// On capte les messages reçus
	while(fgets(message, 512, f) != NULL){
		// On affiche le message
		printf("[Reçu] %s", message);

		// Si la ligne est vide, on sort de la boucle
		if(strcmp(message, "\r\n") == 0 || strcmp(message, "\n") == 0){
			break;
		}

		// On le concatène à la suite des autres messages précédent, en attandant la fin de la requête
		strcat(lignesMessage, message);
	}

	// On traite l'information reçue
	printf("[Info] Traitement de la requête\n");

	// On découpe les chaînes contaténées par des \r\n
	char ** datas = split(lignesMessage, "\r\n", 0);
	char * ressource = traitement_first_line(datas[0]);

	// On traite la premère ligne de la requête (par ex GET / HTTP/1.1)
	if(ressource == NULL){
		fprintf(f, "HTTP/1.1 400 Bad request\nConnection: close.\nContent-Length: %d\n\n400 Bad request\n", (int) strlen(lignesMessage));
		printf("[Info] Traitement interrompu (400 Bad request)\n--------------------\n");
		free(message);
		free(lignesMessage);
		return;
	} else if (strcmp(ressource, "/") == 0){
		fprintf(f, "HTTP/1.1 200 OK\nContent-Length: %d\n\n----- THUNDERWEB -----\nBienvenue sur notre serveur Web.\nCeci est notre message de bienvenue !\nBonne visite ;)\n", (int) strlen(lignesMessage));
	} else {
		fprintf(f, "HTTP/1.1 404 Not found\nContent-Length: %d\n\n404 Not found\n", (int) strlen(lignesMessage));
		printf("[Info] Traitement interrompu (404 Not found)\n--------------------\n");
		free(message);
		free(lignesMessage);
		return;
	}

	// Message de bienvenue

	printf("[Info] Traitement terminé\n--------------------\n");

	// On libère la mémoire utilisée
	free(ressource);
	free(message);
	free(lignesMessage);
}

char * traitement_first_line(const char * req){
	// On sépare les 3 parties de la ligne (délémitées par des espaces)
	char ** tab = split(req, " ", 0);
	int i = 0;

	// Tant que nous sommes pas à la fin de la ligne, on incrémente un compteur
	while(tab[i] != NULL) {
		i++;
	}

	// On teste si la ligne a bien 3 parties distinctes
	if(i != 3){
		printf("[Warning] Requête invalide : nombre de mots invalide\n");
		free(tab);
		return NULL;
	}

	// On teste si le premier mot est GET
	if(strcmp(tab[0], "GET") != 0){
		printf("[Warning] Requête invalide : le premier paramètre est différent de GET\n");
		free(tab);
		return NULL;
	}

	// On teste si la version est bien HTTP/1.0 ou HTTP/1.1
	if(strncmp(tab[2], "HTTP/1.1", 8) != 0 && strncmp(tab[2], "HTTP/1.0", 8) != 0){
		printf("[Warning] Requête invalide : Version invalide\n");
		free(tab);
		return NULL;
	}

	char * ressource = tab[1];

	free(tab);
	return ressource;
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
	printf("[Info] Liaison entre les interfaces et la socket\n");
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
	printf("[Info] Liaison terminée\n");
}