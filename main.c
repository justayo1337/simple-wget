#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>

#define SITE_NAME_SIZE 2048 
void print_addrinfo(struct addrinfo *h);

int main (int argc, char **argv){
	
	if (argc < 3) {
		fprintf(stderr, "Usage: ./simple-wget host port");
		exit(EXIT_FAILURE);
	}
	char sitename[SITE_NAME_SIZE];
	strcpy(sitename,argv[1]);
	int sock = socket(AF_INET, SOCK_STREAM,0);

	//struct sockaddr_in d;
	//d.sin_family = AF_INET;
//	d.sin_port = ntohs(argv[2]);
	struct addrinfo *hints = malloc(sizeof(struct addrinfo));
	memset(hints,0,sizeof(hints));
	struct addrinfo *res,*s;

	hints->ai_family = AF_INET;
	hints->ai_socktype = 0;
	hints->ai_protocol = 0;
	hints->ai_flags  = 0 ;
	int getinfo = getaddrinfo(argv[1],argv[2],hints,&res);
	if (getinfo < 0){
		perror("getaddrinfo: ");
		exit(EXIT_FAILURE);
	}

	s = res;
	while (s->ai_next) {
		print_addrinfo(s);
		printf("\n");
		s = s->ai_next;
	}
	free(hints);
	free(res);
	return 0;

}



void print_addrinfo(struct addrinfo *h){
	printf("FLAGS: %d\n", h->ai_flags);

	char fam[512];
	char sockt[512];
	char *dst = malloc(INET6_ADDRSTRLEN);
	switch(h->ai_family){
		case AF_INET:
			strcpy(fam,"IPv4");
			break;
		case AF_INET6:
			strcpy(fam,"IPv6");	
			break;
		default:
			strcpy(fam,"NULL");
	}

	printf("ADDR_FAM: %s\n", fam);
	switch(h->ai_socktype){
		case SOCK_DGRAM:
			strcpy(sockt,"SOCK_DGRAM");
			break;
		case SOCK_STREAM:
			strcpy(sockt,"SOCK_STREAM");	
			break;
		default:
			strcpy(sockt,"NULL");
	}
	printf("SOCKET_TYPE: %s\n", sockt);
	printf("PROTO: %d\n", h->ai_protocol);
	printf("ADDR_LEN: %u\n", h->ai_addrlen);

	if (h->ai_addrlen == 16){
//		((struct sockaddr_in *) h->ai_addr)->sin_addr.s_addr = ntohl(((struct sockaddr_in *) h->ai_addr)->sin_addr.s_addr );
		inet_ntop(AF_INET,&((struct sockaddr_in *) h->ai_addr)->sin_addr,dst,INET_ADDRSTRLEN);
		printf("ADDR: %s\n", dst);
	}else{

//		((struct sockaddr_in6 *) h->ai_addr)->sin6_addr.s6_addr = ntohl(((struct sockaddr_in6 *) h->ai_addr)->sin6_addr.s6_addr );
		inet_ntop(AF_INET6,&((struct sockaddr_in6 *) h->ai_addr)->sin6_addr,dst,INET6_ADDRSTRLEN);
		printf("ADDR: %s\n", dst);
	}
	printf("CANON_NAME: %s\n", h->ai_canonname);
	free(dst);
}
