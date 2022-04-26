#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include "client.h"
#include <arpa/inet.h>
struct sockaddr_in serv_addr, client_addr;
int main()
{
	functie(); 
	FILE *fisier;
	pid_t childpid;
	socklen_t size_client;
	int sockfd, new_socket;
	char buffer[256];
	int retval;
	int val;
	int socket_client;
	char *mesaj = "comanda neimplementata" ;
	char *mesaj_bun = "comanda implementata " ;
       /* Stabilire canal de comunicare */	
	sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd == -1) {
		perror("socket()");
		exit(1);
	}
        /* Address Family pentru IPV4 */
	serv_addr.sin_family = AF_INET;
	/* Clientul ipv4 se va putea conecta la toate adresele asignate serverului IPV4 */
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	/* Realizeaza conversia portului din little endian in big endian */
	serv_addr.sin_port =htons(22115);
        /* Se ataseaza adresa ip si portul la sockfd ( creat anterior cu functia socket () ) */
	if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1) { 
		perror("bind()");
		exit(1);
	}
        /* Se verifica disponibilitatea serverului pentru acceptarea conexiunilor */
	/* Coada de asteptare necesara in cazul in care mai multi clienti doresc sa se conecteze in acelasi timp are dimensiunea 5 */
	if (listen(sockfd,5) == -1) {
		perror("listen()");
		exit(1);
	}


	while(1) {	
		size_client  = sizeof(client_addr);
		/*Clientul ipv6 care doreste sa se conecteze la server este pus in asteptare pana cand conexiunea este stabilita */
		/* Odata conectat, se creeaza un socket nou pentru clientul respectiv */
		new_socket = accept(sockfd,(struct sockaddr *)&client_addr,&size_client);
		if (new_socket == -1) {
			exit(1);
		}
	       /* Functia inet_ntoa converteste adresa primita in binar intr-o adresa ipv4 standard  in format decimal si cu . */
	       /* Functia ntohs converteste din format big endian in little endian ( inversul lui htons ) */	
		printf("Clientul cu adresa %s:%d s-a CONECTAT \n", inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));

		/* Scopul functiei fork este de a crea un nou proces, care devine child pentru procesul principal, in cazul de fata serverul devine multi-client si poate servi multiplii clienti simultan */
		if ((childpid = fork()) == 0) /* se creeaza un nou proces */
	       	{
			close(sockfd);  
			while(1) {
				/* se primesc date utilizand canalul unic de comunicare */
				retval = recv(new_socket, buffer, sizeof(buffer),0);
				if (retval == -1 ){
					perror("recv()");
					exit(1);
				}
				if(retval == 0) {
					printf("Clientul cu adresa %s:%d s-a DECONECTAT \n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
					break;
				}
				if (retval > 0)
				{
					printf("Clientul cu adresa %s a cerut comanda : %s\n", inet_ntoa(client_addr.sin_addr),  buffer);
					/* Comanda ceruta de client se trimite folosind functia send, pe canalul de comunicare new_socket */
					send(new_socket, buffer, strlen(buffer), 0);
                                        /* Comanda implementata -> 01# */
					/* Atunci cand clientul cere comanda implementata serverul trimite pagina in format html */
					if (strncmp(buffer,"01#",3) == 0 ) {
						fisier = fopen("index.html", "r");
						char sir[255];
						while(!feof(fisier)){
							fgets(sir,255,fisier);
							char *continut = sir;
							send(new_socket, continut, strlen(continut), 0);

						}
						fclose(fisier);
					}
					else
					{
						val = send( new_socket, mesaj, strlen(mesaj), 0); // mesaj pentru comanda neimplementata
					}
				}
				if( val == -1) {
					perror("send()");
					break;
				}  
			}
	       	}
		close(new_socket);
	}
	return 0;
}
