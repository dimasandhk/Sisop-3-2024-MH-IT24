void
logs(char *actor, char *msg)
{
	char 		strtime[100], log[100] = {0};
	time_t		tm	= time(NULL);
	struct tm 	*local	= localtime(&tm);
	FILE 		*fp 	= fopen("race.log", "a+");
	
	strftime(strtime, sizeof(strtime), "%d/%m/%Y %T", local);
	snprintf(log, sizeof(log), "[%s][%s]: %s", actor, strtime, msg);
	
	printf("%s", log);
	fputs(log, fp);
	fclose(fp);
}

void
gap(float time, char *msg)
{	
	if (time < 3.5) {
		sprintf(msg, "[Paddock]: Gogogo\n");
		logs("Paddock", "Gogogo\n");
	} else if (time  >= 3.5 && time < 10) {
		sprintf(msg, "[Paddock]: Push\n");
		logs("Paddock", "Push\n");
	} else if (time >= 10){
		sprintf(msg, "[Paddock]: Stay out of trouble\n");
		logs("Paddock", "Stay out of trouble\n");
	}
}

void
fuel(float fuel, char *msg)
{	
	if (fuel >= 80) {
		sprintf(msg, "[Paddock]: Push Push Push\n");
		logs("Paddock", "Push Push Push\n");
	} else if (fuel  < 80 && fuel > 50) {
		sprintf(msg, "[Paddock]: You can go\n");
		logs("Paddock", "You can go\n");
	} else if (fuel <= 50){
		sprintf(msg, "[Paddock]: Conserve fuel\n");
		logs("Paddock", "Conserve fuel\n");
	}
}

void
tire(int tire, char *msg)
{	
	if (tire >= 80) {
		sprintf(msg, "[Paddock]: Go Push Go Push\n");
		logs("Paddock", "Go Push Go Push\n");
	} else if (tire < 80 && tire > 50) {
		sprintf(msg, "[Paddock]: Good Tire Wear\n");
		logs("Paddock", "Good Tire Wear\n");
	} else if (tire <= 50){
		sprintf(msg, "[Paddock]: Conserve tire\n");
		logs("Paddock", "Conserve tire\n");
	}
}

void
tire_change(char *type, char *msg)
{	
	if ( !(strcmp(type, "soft")) ) {
		sprintf(msg, "[Paddock]: Mediums ready\n");
		logs("Paddock", "Mediums ready\n");
	} else if ( !(strcmp(type, "medium")) ) {
		sprintf(msg, "[Paddock]: Box for soft\n");
		logs("Paddock", "Box for soft\n");
	} else {
		bzero(msg, sizeof(msg));
	}
}

int
be_daemon()
{
	int	i;
	pid_t	pid;
	
	if ( (pid = fork()) < 0)
		return (-1);	
	else if(pid)
		exit(0);	/* kill parent */

	if (setsid() < 0)	/* child 1 become session leader */
		return (-1);	

	signal(SIGHUP, SIG_IGN);
	if ( (pid = fork()) < 0)
		return (-1);
	else if (pid)
		exit(0);	/* kill child 1 */

	chdir("/");		/* change working directory */

	/* close off file descriptors */
	for (i = 0; i < MAXFD; i++)
		close(i);

	/* redirect stdin,stdout,stderr to /dev/null */
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

	return (0);
}
