int serveloop(SERVER* serve) {
	struct sockaddr_in addrin;
	socklen_t addrinlen=sizeof(addrin);
	for(;;) {
		CLIENT *client;
		int net;
		pid_t *pid;

		DEBUG("accept, ");
		if ((net = accept(serve->socket, (struct sockaddr *) &addrin, &addrinlen)) < 0) {
			msg2(LOG_ERR,"accept: %m");
			continue;
		}

		client = g_malloc(sizeof(CLIENT));
		client->server=serve;
		client->exportsize=OFFT_MAX;
		client->net=net;
		set_peername(net, client);
		if (!authorized_client(client)) {
			msg2(LOG_INFO,"Unauthorized client") ;
			close(net) ;
			continue ;
		}
		msg2(LOG_INFO,"Authorized client") ;
		pid=g_malloc(sizeof(pid_t));
#ifndef NOFORK
		if ((*pid=fork())<0) {
			msg3(LOG_INFO,"Could not fork (%s)",strerror(errno)) ;
			close(net) ;
			continue ;
		}
		if (*pid>0) { /* parent */
			close(net);
			g_hash_table_insert(children, pid, pid);
			continue;
		}
		/* child */
		g_hash_table_destroy(children);
		close(serve->socket) ;
#endif // NOFORK
		msg2(LOG_INFO,"Starting to serve") ;
		serveconnection(client);
	}
}