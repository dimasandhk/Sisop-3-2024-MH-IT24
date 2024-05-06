/* 
 * -- SERVER SIDE --
 * process requests
 * listen to client
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include "actions.c"	 /* to outsource functions */

/* socket related */
#include <sys/socket.h>	
#include <netinet/in.h>
#include <arpa/inet.h>
#define	 MAXLINE 	1024
#define	 SA		struct sockaddr
#define	 PORT		51184

struct sockaddr_in servaddr;
int	listenfd, connfd;
char	buff[MAXLINE], argv1[50], argv2[50];

int
main()
{
	listenfd = socket(AF_INET, SOCK_STREAM, 0);	/* socket creation */
	
	/* bound server port to socket */
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family	 = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);	/*connect to any interface */
	servaddr.sin_port	 = htons(PORT);
	
	bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	listen(listenfd, 1024);		/* turn sock to listening sock*/
	
	
	/* accept client connection and response */
	for ( ; ; ) {
		connfd = accept(listenfd, (SA *) NULL, NULL);		
		
		/* process input */
		read(connfd, buff, sizeof(buff));
		sscanf(buff, "%[^ ] %[^\n]\n", argv1, argv2);

		if ( !(strcmp(argv1, "fuel")) )
			logs("Driver", buff);
			gap(atof(argv2), buff);


		
		//snprintf(buff, sizeof(buff), "sax");

		write(connfd, buff, strlen(buff));
		close(connfd);		/* close connection */
	}

}


