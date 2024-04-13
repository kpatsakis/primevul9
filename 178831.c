static char* ReadPassword(rfbClient* client) {
	int i;
	char* p=calloc(1,9);
	if (!p) return p;
#ifndef WIN32
	struct termios save,noecho;
	if(tcgetattr(fileno(stdin),&save)!=0) return p;
	noecho=save; noecho.c_lflag &= ~ECHO;
	if(tcsetattr(fileno(stdin),TCSAFLUSH,&noecho)!=0) return p;
#endif
	fprintf(stderr,"Password: ");
	fflush(stderr);
	i=0;
	while(1) {
		int c=fgetc(stdin);
		if(c=='\n')
			break;
		if(i<8) {
			p[i]=c;
			i++;
			p[i]=0;
		}
	}
#ifndef WIN32
	tcsetattr(fileno(stdin),TCSAFLUSH,&save);
#endif
	return p;
}