void
logs(char *source, char *msg)
{
	time_t		tm	= time(NULL);
	char 		strtime[100];
	struct tm 	*local	= localtime(&tm);
	FILE 		*fp 	= fopen("race.log", "r+");
	
	strftime(strtime, sizeof(strtime), "%e/%m/%Y %T", local);
	printf("%s", strtime);
}

void
gap(float time, char *msg)
{
	
	if (time < 3.5) {
		sprintf(msg, "Gogogo"); 
	} else if (time  > 3.5 && time < 10) {
		sprintf(msg, "Push");
	} else if (time > 10){
		sprintf(msg, "Stay out of trouble");
	} else {
		printf("[!] Negative time value");	/* negative time value error */
	}
}


