void setup_serve(SERVER* serve) {
	struct sockaddr_in addrin;
	struct sigaction sa;
	int addrinlen = sizeof(addrin);
#ifndef sun
	int yes=1;
#else
	char yes='1';
#endif /* sun */

	if ((serve->socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
		err("socket: %m");

	/* lose the pesky "Address already in use" error message */
	if (setsockopt(serve->socket,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
	        err("setsockopt SO_REUSEADDR");
	}
	if (setsockopt(serve->socket,SOL_SOCKET,SO_KEEPALIVE,&yes,sizeof(int)) == -1) {
		err("setsockopt SO_KEEPALIVE");
	}

	DEBUG("Waiting for connections... bind, ");
	addrin.sin_family = AF_INET;
	addrin.sin_port = htons(serve->port);
	addrin.sin_addr.s_addr = 0;
	if (bind(serve->socket, (struct sockaddr *) &addrin, addrinlen) < 0)
		err("bind: %m");
	DEBUG("listen, ");
	if (listen(serve->socket, 1) < 0)
		err("listen: %m");
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGCHLD, &sa, NULL) == -1)
		err("sigaction: %m");
	sa.sa_handler = sigterm_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if(sigaction(SIGTERM, &sa, NULL) == -1)
		err("sigaction: %m");
	children=g_hash_table_new_full(g_int_hash, g_int_equal, NULL, destroy_pid_t);
}