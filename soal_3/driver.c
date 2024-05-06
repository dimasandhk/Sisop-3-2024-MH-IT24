/* 
 * -- CLIENT SIDE --
 * ask for what-to-do
 * server replied
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* socket related */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define	 MAXLINE	1024
#define	 SA		struct sockaddr
#define	 PORT		51184

struct sockaddr_in servaddr;
int	sockfd, n;
char	buff[MAXLINE];

int
main()
{
	if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)	/* create tcp connection */
		printf("[-]socket generation\n");

	/* specify IP address and port  */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family 	= AF_INET;
	servaddr.sin_port 	= htons(PORT);
	if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0)
		printf("[-]inet_pton error\n");

	if (connect(sockfd, (SA *) &servaddr, sizeof(servaddr)) < 0) /* connect to server */
		printf("[-]connect error\n");

	/* sending command to server */
	fgets(buff, sizeof(buff), stdin);
	write(sockfd, buff, strlen(buff));

	/* read server response */
	while ( (n = read(sockfd, buff, MAXLINE) ) > 0) {
		buff[n] = 0;
		if (fputs(buff, stdout) == EOF)
			printf("[-]fputs error\n");
	}	
	if (n < 0)
		printf("[-]read error\n");


	exit(0);
}
