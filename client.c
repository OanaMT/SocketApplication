#include <stdio.h>
#include <string.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#define serverport 80
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include "client.h"
/* Structura care contine informatii despre adresa serverului IPV6 */
struct sockaddr_in6 server_addr;
char buffer[512]; 
int octeti_recv;
int lungime_mesaj;
int octeti;
int functie(){

	FILE *fisier;
	/* Stocare mesaj aferent cererii HTTP */
	char *mesaj2="GET /HTTP/1.0\r\n\r\n"; 
	fisier = fopen("index.html","w");
	int s = -1;
	/* Initializare functie socket pentru a crea un nou canal de comunicare. */
	/* Parametrii functiei : AF_INET6 -> Address Family, folosita pentru adrese pe 128 biti */
	/* SOCK_STREAM -> tip de apel, bazat pe protocolul TCP ( IPPROTO_TCP ) */
	s = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
	
	if (s == -1){
		perror("socket()");
	}
	else printf("Socket client disponibil \n ");
 
	
	server_addr.sin6_family = AF_INET6;
	/* Functia inet_pton realizeaza conversia adresei ipv6 ( in acest caz ) din text in format binar */
	inet_pton(AF_INET6, "2a00:1288:110:c305::1:8001", &server_addr.sin6_addr);
	/* Functia htons realizeaza conversia din format little endian(unsigned short integer)  in format big endian, recunoscut de TCP/IP */
	server_addr.sin6_port = htons(serverport);

	int c;
	/* Functia connect incearca sa stabileasca o conexiune intre client si server . Procesul este blocat pana cand conexiunea este stabilita */
	c = connect(s, (struct sockaddr*)&server_addr, sizeof(server_addr));

	if (c == -1) {
		perror("connect()");
		close(s);
	}
	else printf("Se pot realiza conexiuni \n"); 


	lungime_mesaj=strlen(mesaj2);

        /*Functia send returneaza nr de octeti trimisi prin conexiunea stabilita anterior, in mod full duplex */
	octeti = send(s,mesaj2,lungime_mesaj,0);
	
	if (octeti == -1 ) {
		perror("send()"); 
	}
	else printf("Cerere http trimisa cu succes\n");
        
	int ok = 0;
	/* Secventa de mai jos se executa pana cand nu mai sunt caractere de primit*/
	do {    
		/* Functia recv returneaza numarul de octeti cititi din buffer */
		octeti_recv =recv(s,buffer,sizeof buffer,0);
		if ( octeti > 0)
		{       /* Scriere in fisier */
			for (int i=0;i<=octeti_recv;i++)
				fprintf(fisier,"%c", buffer[i]);
		}
	ok = 1;
	}while(octeti_recv > 0);
         if (ok == 1) 
		 printf("Mesajul a fost primit \n");
	 if (octeti_recv == -1 ) 
		perror("recv()");
	 if (octeti_recv == 0 && ok==0 )
		printf("Nu sunt mesaje disponibile de receptionat\n" );

		
	fclose(fisier);
	/* se inchide canalul de comunicare dupa ce s-au trimis si primit date */
	close(s);
	return 0;
}
