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
#include <signal.h>
#include <fcntl.h>

/* socket related */
#include <sys/socket.h>	
#include <netinet/in.h>
#include <arpa/inet.h>
#define	 MAXFD		64
#define	 MAXLINE 	1024
#define	 PORT		62192
#define	 SA		struct sockaddr

#include "actions.c"	 /* to outsource functions */

struct sockaddr_in servaddr;
int	listenfd, connfd;
char	buff[MAXLINE], argv1[50], argv2[50];

int
main()
{
	if (be_daemon() != 0)
		printf("[-]daemon error");		/* turn to daemon*/
	
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

		if ( !(strcmp(argv1, "gap")) ) {
			logs("Driver", buff);
			gap(atof(argv2), buff);
		} else if ( !(strcmp(argv1, "fuel")) ) {
			logs("Driver", buff);
			fuel(atof(argv2), buff);
		} else if ( !(strcmp(argv1, "tire")) ) {
			logs("Driver", buff);
			tire(atof(argv2), buff);
		}  else if ( !(strcmp(argv1, "tire-change")) ) {
			logs("Driver", buff);
			tire_change(argv2, buff);
		} else {
			bzero(buff, sizeof(buff));
		}

		write(connfd, buff, strlen(buff));
		bzero(buff, sizeof(buff));
		close(connfd);		/* close connection */
	}

}


