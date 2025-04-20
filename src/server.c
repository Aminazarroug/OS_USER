// Nous complétons le code qui a déjà été donné 

 #include <stdio.h>
 #include <stdlib.h>
 #include <string.h>
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <netinet/in.h>
 
 #include <netdb.h>
 #include <arpa/inet.h>
 #include <time.h>
 
 struct _client {
     char ipAddress[40];		// Adresse ip
     int port;				// Numéro de port
     char name[40];			// Nom d'utilisateur
 } tcpClients[4];
 int nbClients;
 int fsmServer;	// Etat du serveur : 0 (attente de connexion) ou 1 (jeu)
 int deck[13] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};	// Cartes
 int tableCartes[4][8];	// Table des symboles (4 joueurs, 8 symboles)
 char *nomcartes[] = {
     "Sebastian Moran","Irene Adler","Inspector Lestrade","Inspector Gregson",
     "Inspector Baynes","Inspector Bradstreet","Inspector Hopkins","Sherlock Holmes",
     "John Watson","Mycroft Holmes","Mrs. Hudson","Mary Morstan","James Moriarty"};
 int joueurCourant;
 int elimine[4];	
 int jrestants;	 // joueurs restans
 int jrejouant;	 // joueurs rejouants dans la partie
 
 void error(const char *msg) {
     perror(msg);
     exit(1);
 }
 
 void melangerDeck() {
     int i;
     int index1, index2, tmp;
 
     for (i = 0 ; i < 1000 ; i++) {
         index1 = rand() % 13;
         index2 = rand() % 13;
 
         tmp = deck[index1];
         deck[index1] = deck[index2];
         deck[index2] = tmp;
     }
 }
 
 void createTable() { // Permet l'initialisation du tableau des joueurs
    
     int i, j, c;
 
     for (i = 0 ; i < 4 ; i++)
         for (j = 0 ; j < 8 ; j++)
             tableCartes[i][j] = 0;
 
     for (i = 0 ; i < 4 ; i++) { // i = numero du joueur
         for (j = 0 ; j < 3 ; j++) {
             c = deck[i * 3 + j];
             switch (c) {
                 case 0: // Sebastian Moran
                     tableCartes[i][7]++; tableCartes[i][2]++;
                     break;
                 case 1: // Irene Adler
                     tableCartes[i][7]++;
                     tableCartes[i][1]++;
                     tableCartes[i][5]++;
                     break;
                 case 2: // Inspector Lestrade
                     tableCartes[i][3]++;
                     tableCartes[i][6]++;
                     tableCartes[i][4]++;
                     break;
                 case 3: // Inspector Gregson 
                     tableCartes[i][3]++;
                     tableCartes[i][2]++;
                     tableCartes[i][4]++;
                     break;
                 case 4: // Inspector Baynes 
                     tableCartes[i][3]++;
                     tableCartes[i][1]++;
                     break;
                 case 5: // Inspector Bradstreet 
                     tableCartes[i][3]++;
                     tableCartes[i][2]++;
                     break;
                 case 6: // Inspector Hopkins 
                     tableCartes[i][3]++;
                     tableCartes[i][0]++;
                     tableCartes[i][6]++;
                     break;
                 case 7: // Sherlock Holmes 
                     tableCartes[i][0]++;
                     tableCartes[i][1]++;
                     tableCartes[i][2]++;
                     break;
                 case 8: // John Watson 
                     tableCartes[i][0]++;
                     tableCartes[i][6]++;
                     tableCartes[i][2]++;
                     break;
                 case 9: // Mycroft Holmes 
                     tableCartes[i][0]++;
                     tableCartes[i][1]++;
                     tableCartes[i][4]++;
                     break;
                 case 10: // Mrs. Hudson 
                     tableCartes[i][0]++;
                     tableCartes[i][5]++;
                     break;
                 case 11: // Mary Morstan 
                     tableCartes[i][4]++;
                     tableCartes[i][5]++;
                     break;
                 case 12: // James Moriarty 
                     tableCartes[i][7]++;
                     tableCartes[i][1]++;
                     break;
             }
         }
     }
 }
 
 void printDeck() { // permet d'afficher le tableau des cartes
     int i, j;
     for (i = 0 ; i < 13 ; i++)
         printf("%d %s\n", deck[i], nomcartes[deck[i]]);
 
     for (i = 0 ; i < 4 ; i++) {
         for (j = 0 ; j < 8 ; j++)
             printf("%2.2d ", tableCartes[i][j]);
         puts("");
     }
 }
 
 void printClients() {  // pour qu'on puisse voir les informations des clients
     int i;
     for (i = 0 ; i < nbClients ; i++)
         printf("%d: %s %5.5d %s\n",
                 i,
                 tcpClients[i].ipAddress,
                 tcpClients[i].port,
                 tcpClients[i].name);
 }
 
 int findClientByName(char *name) { // Retourne à partir du nom du joueur l'indice correspondant 
     int i;
     for (i = 0 ; i < nbClients ; i++)
         if (strcmp(tcpClients[i].name, name) == 0)
             return i;
     return -1;
 }
 
 void sendMessageToClient(char *clientip, int clientport, char *mess) {
     int sockfd, portno, n;
     struct sockaddr_in serv_addr;
     struct hostent *server;
     char buffer[256];
 
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
 
     server = gethostbyname(clientip);
     if (server == NULL) {
         fprintf(stderr, "ERROR, no such host\n");
         exit(0);
     }
     bzero((char *) &serv_addr, sizeof(serv_addr));
     serv_addr.sin_family = AF_INET;
     bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
     serv_addr.sin_port = htons(clientport);
     if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
         printf("ERROR connecting\n");
         exit(1);
     }
 
     sprintf(buffer, "%s\n", mess);
     n = write(sockfd, buffer, strlen(buffer));
 
     close(sockfd);
 }
 
 void broadcastMessage(char *mess) { // permet d'envoyer un message à tous les clients
     int i;
     for (i = 0 ; i < nbClients ; i++)
         sendMessageToClient(
             tcpClients[i].ipAddress,
             tcpClients[i].port,
             mess);
 }
 
 int nextPlayer(int joueurCourant) {
     int p = (joueurCourant + 1) % 4;
     while (elimine[p]) {
         p = (p + 1) % 4;
     }
     return p;
 }
 
 void initGame() { // début d'une partie
     printDeck();
     melangerDeck();
     createTable();
     printDeck();
     joueurCourant = 0;
     jrestants = 4;        // On rajoute cette partie pour faciliter la fin du jeu
     for (int i = 0 ; i < 4 ; i++)
         elimine[i] = 0;
 }
 
 int main(int argc, char *argv[]) {
     srand(time(NULL)); // initialisation de rand
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     char reply[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     int i, j;
 
     char com;
     char clientIpAddress[256], clientName[256];
     int clientPort;
     int id;
 
     if (argc < 2) {	
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0)
         error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) 
         error("ERROR on binding");
     listen(sockfd, 5);
     clilen = sizeof(cli_addr);
 
     initGame();
 
     for (i = 0 ; i < 4 ; i++) {
         strcpy(tcpClients[i].ipAddress, "localhost");
         tcpClients[i].port = -1;
         strcpy(tcpClients[i].name, "-");
     }
 
     while (1) {
         newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen); // Attend la connexion d'un client
         if (newsockfd < 0)
             error("ERROR on accept");
         printf("New client!\n");
 
          bzero(buffer, 256);
          n = read(newsockfd, buffer, 255);
          if (n < 0)
             error("ERROR reading from socket");
 
         printf("Received packet from %s:%d\nData: %s\n",
                 inet_ntoa(cli_addr.sin_addr), ntohs(cli_addr.sin_port), buffer);
 
         if (fsmServer == 0) {	// On attend 4 connexions
             switch (buffer[0]) {
                 case 'C':	// Message 'C' : Connexion
                     sscanf(buffer, "%c %s %d %s", &com, clientIpAddress, &clientPort, clientName);
                     printf("COM=%c ipAddress=%s port=%d name=%s\n", com, clientIpAddress, clientPort, clientName);
 
                     strcpy(tcpClients[nbClients].ipAddress, clientIpAddress);
                     tcpClients[nbClients].port = clientPort;
                     strcpy(tcpClients[nbClients].name,clientName);
                     nbClients++;
 
                     printClients();
  
                     id = findClientByName(clientName);
                     printf("id=%d\n",id);
  
                     sprintf(reply, "I %d", id);		// Message 'I' : Id
                     sendMessageToClient(tcpClients[id].ipAddress, tcpClients[id].port, reply);
 

 
                     sprintf(reply, "L %s %s %s %s",
                         tcpClients[0].name,
                         tcpClients[1].name,
                         tcpClients[2].name,
                         tcpClients[3].name); 
                     broadcastMessage(reply);
 
                     // Si on a 4 joueurs alors on peut commencer la partie
 
                     if (nbClients == 4) {
                         for (i = 0 ; i < 4 ; i++) {
                             sprintf(reply, "D %d %d %d", deck[0 + 3*i], deck[1 + 3*i], deck[2 + 3*i]);
                             sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, reply);
                             for (j = 0 ; j < 8 ; j ++) {
                                 sprintf(reply, "V %d %d %d", i, j, tableCartes[i][j]);
                                 sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, reply);
                             }
                         }
 
                         sprintf(reply, "M %d", joueurCourant);
                         broadcastMessage(reply);
                         fsmServer = 1;
                     }
                 break;
             }
         }
         else if (fsmServer == 1) {	
             switch (buffer[0]) {
                 case 'G':	// Message 'G' : Guilt coupable
                     sscanf(buffer, "G %d %d", &id, &i);
                     if (i == deck[12]) {	//Si réellement coupable
                         sprintf(reply, "W %d %d", id, i);
                         jrejouant = 0;
                         broadcastMessage(reply);
                     }
                     else {	// Erreur
                         sprintf(reply, "E %d %d", id, i);
                         broadcastMessage(reply);
                         jrestants--;
                         elimine[id] = 1;
                         if (jrestants == 1) {	// il gagne 
                             sprintf(reply, "W %d %d", nextPlayer(joueurCourant), deck[12]);
                             jrejouant = 0;
                         }
                         else {	// dans les autres cas on continue
                             joueurCourant = nextPlayer(joueurCourant);
                             sprintf(reply, "M %d", joueurCourant);
                         }
                         broadcastMessage(reply);
                     }
                     break;
                 case 'O':	// Message 'O' : Others tout le monde
                     sscanf(buffer, "O %d %d", &id, &j);
                     for (i = 0 ; i < 4 ; i++) {
                         if (i != id) {
                             if (tableCartes[i][j] > 0) {
                                 sprintf(reply, "V %d %d 100", i, j);
                             }
                             else {
                                 sprintf(reply, "V %d %d 0", i, j);
                             }
                             broadcastMessage(reply);
                         }
                     }
                     joueurCourant = nextPlayer(joueurCourant);
                     sprintf(reply, "M %d", joueurCourant);
                     broadcastMessage(reply);
                     break;
                 case 'S':	// Message 'S' : Solo 
                     sscanf(buffer, "S %d %d %d", &id, &i, &j);
                     sprintf(reply, "V %d %d %d", i, j, tableCartes[i][j]);
                     broadcastMessage(reply);
                     joueurCourant = nextPlayer(joueurCourant);
                     sprintf(reply, "M %d", joueurCourant);
                     broadcastMessage(reply);
                     break;
                 case 'H':	// Message 'H' : Head on envoi un emoji (plus)
                     sscanf(buffer, "H %d %d", &id, &i);
                     sprintf(reply, "H %d %d", id, i);
                     broadcastMessage(reply);
                     break;
                 case 'R':	// Message 'R' : Replay on demande à un joueur de rejouer
                     sscanf(buffer, "R %d", &id);
                     sprintf(reply, "R %d", id);
                     broadcastMessage(reply);
                     jrejouant++;
                     if (jrejouant == 4) {	// Si les 4 joueurs veulent rejouer
                         // Création d'une nouvelle partie
                         initGame();
                         for (i = 0 ; i < 4 ; i++) {
                             sprintf(reply, "D %d %d %d", deck[0 + 3*i], deck[1 + 3*i], deck[2 + 3*i]);
                             sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, reply);
                             for (j = 0 ; j < 8 ; j ++) {
                                 sprintf(reply, "V %d %d %d", i, j, tableCartes[i][j]);
                                 sendMessageToClient(tcpClients[i].ipAddress, tcpClients[i].port, reply);
                             }
                         }
                         sprintf(reply, "M %d", joueurCourant);
                         broadcastMessage(reply);
                     }
                     break;
                 default:
                     break;
             }
         }
         close(newsockfd);
     }
     close(sockfd);
     return 0;
 }