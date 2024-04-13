void daemonize(SERVER* serve) {
	FILE*pidf;

	if((serve->port)) {
		if(daemon(0,0)<0) {
			err("daemon");
		}
		snprintf(pidfname, sizeof(char)*255, "/var/run/nbd-server.%d.pid", serve->port);
		pidf=fopen(pidfname, "w");
		if(pidf) {
			fprintf(pidf,"%d", (int)getpid());
			fclose(pidf);
		} else {
			perror("fopen");
			fprintf(stderr, "Not fatal; continuing");
		}
	}
}