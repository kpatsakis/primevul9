get_worker(ns_clientmgr_t *manager, ns_interface_t *ifp, isc_socket_t *sock)
{
	isc_result_t result = ISC_R_SUCCESS;
	isc_event_t *ev;
	ns_client_t *client;
	MTRACE("get worker");

	REQUIRE(manager != NULL);

	if (manager->exiting)
		return (ISC_R_SHUTTINGDOWN);

	/*
	 * Allocate a client.  First try to get a recycled one;
	 * if that fails, make a new one.
	 */
	client = NULL;
	if (!ns_g_clienttest)
		ISC_QUEUE_POP(manager->inactive, ilink, client);

	if (client != NULL)
		MTRACE("recycle");
	else {
		MTRACE("create new");

		LOCK(&manager->lock);
		result = client_create(manager, &client);
		UNLOCK(&manager->lock);
		if (result != ISC_R_SUCCESS)
			return (result);

		LOCK(&manager->listlock);
		ISC_LIST_APPEND(manager->clients, client, link);
		UNLOCK(&manager->listlock);
	}

	client->manager = manager;
	ns_interface_attach(ifp, &client->interface);
	client->newstate = client->state = NS_CLIENTSTATE_WORKING;
	INSIST(client->recursionquota == NULL);
	client->tcpquota = &ns_g_server->tcpquota;

	client->dscp = ifp->dscp;

	client->attributes |= NS_CLIENTATTR_TCP;
	client->pipelined = true;
	client->mortal = true;

	isc_socket_attach(ifp->tcpsocket, &client->tcplistener);
	isc_socket_attach(sock, &client->tcpsocket);
	isc_socket_setname(client->tcpsocket, "worker-tcp", NULL);
	(void)isc_socket_getpeername(client->tcpsocket, &client->peeraddr);
	client->peeraddr_valid = true;

	INSIST(client->tcpmsg_valid == false);
	dns_tcpmsg_init(client->mctx, client->tcpsocket, &client->tcpmsg);
	client->tcpmsg_valid = true;

	INSIST(client->nctls == 0);
	client->nctls++;
	ev = &client->ctlevent;
	isc_task_send(client->task, &ev);

	return (ISC_R_SUCCESS);
}