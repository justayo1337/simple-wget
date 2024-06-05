#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#define SITE_NAME_SIZE 2048 

#define DEFAULT_VERB "GET" 
#define DEFAULT_RESOURCE  "/"
static char  sitename[SITE_NAME_SIZE];
static char  outfileName[FILENAME_MAX];
//#define 
void print_addrinfo(struct addrinfo *h);
char * generate_req(char * verb, char * resource);

int  main_request(int sock,struct addrinfo *,char * resource);

int main (int argc, char **argv){
	
	if (argc < 3) {
		fprintf(stderr, "Usage: ./simple-wget host port resource['/']");
		exit(EXIT_FAILURE);
	}
	char resource[2048];
	int sock;
	strcpy(sitename,argv[1]);
	strcpy(resource,DEFAULT_RESOURCE);
	if (argv[3])
		strcpy(resource,argv[3]);
		
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

		sock = socket(s->ai_family,s->ai_socktype ,s->ai_protocol);
		if (sock > 0){
			break;
		}
		s = s->ai_next;
	}
	if (sock < 0){
		perror("socket: could not create the socket");
		exit(EXIT_FAILURE);
	}

	main_request(sock,s,resource);
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


int main_request(int sock, struct addrinfo * addr, char * resource){
	
	char *dst = malloc(INET6_ADDRSTRLEN);
	int c = connect(sock, addr->ai_addr,addr->ai_addrlen);

	if (c < 0){
		perror("connect");
		return c;
	}
	printf("Connected! %d", c);
	char * req = generate_req(DEFAULT_VERB, resource);
	if (addr->ai_addrlen == 16){
//		((struct sockaddr_in *) h->ai_addr)->sin_addr.s_addr = ntohl(((struct sockaddr_in *) h->ai_addr)->sin_addr.s_addr );
		inet_ntop(AF_INET,&((struct sockaddr_in *) addr->ai_addr)->sin_addr,dst,INET_ADDRSTRLEN);
		//printf("ADDR: %s\n", dst);
	}else{

//		((struct sockaddr_in6 *) h->ai_addr)->sin6_addr.s6_addr = ntohl(((struct sockaddr_in6 *) h->ai_addr)->sin6_addr.s6_addr );
		inet_ntop(AF_INET6,&((struct sockaddr_in6 *) addr->ai_addr)->sin6_addr,dst,INET6_ADDRSTRLEN);
		//printf("ADDR: %s\n", dst);
	}
	sprintf(req+strlen(req),"Host: %s\r\n\r\n",dst);
	int r = send(sock,req,strlen(req),0);
	if (r < 0){
		perror("send");
		exit(EXIT_FAILURE);
	}

	char recv_buff[2048];
	while ((r=recv(sock,recv_buff, sizeof(recv_buff),0)) > 0)
		printf("%s\r\n",recv_buff);
	
	if (r < 0 )
		perror("recv");
		exit(EXIT_FAILURE);
	close(c);
	return 0;
}



char * generate_req(char * verb, char * resource){
	char * req = malloc(2048 * sizeof(char));

	sprintf(req,"%s %s HTTP/1.1\r\n", verb,resource);
	sprintf(req+strlen(req),"User-Agent: simple-wget/1.0\r\n");
	sprintf(req+strlen(req),"Host: %s\r\n",sitename);
	sprintf(req+strlen(req),"\r\n");
	printf("\n%s\n",req);
	return req;
}

