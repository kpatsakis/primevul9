int authorized_client(CLIENT *opts) {
	FILE *f ;
   
	char line[LINELEN]; 

	if ((f=fopen(opts->server->authname,"r"))==NULL) {
		msg4(LOG_INFO,"Can't open authorization file %s (%s).",
		     opts->server->authname,strerror(errno)) ;
		return 1 ; 
	}
  
	while (fgets(line,LINELEN,f)!=NULL) {
		if (strncmp(line,opts->clientname,strlen(opts->clientname))==0) {
			fclose(f);
			return 1;
		}
	}
	fclose(f) ;
	return 0 ;
}