get_client(ns_clientmgr_t *manager, ns_interface_t *ifp,
	   dns_dispatch_t *disp, bool tcp)
{
	isc_result_t result = ISC_R_SUCCESS;
	isc_event_t *ev;
	ns_client_t *client;
	MTRACE("get client");

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
	client->state = NS_CLIENTSTATE_READY;
	INSIST(client->recursionquota == NULL);

	client->dscp = ifp->dscp;

	if (tcp) {
		client->attributes |= NS_CLIENTATTR_TCP;
		isc_socket_attach(ifp->tcpsocket,
				  &client->tcplistener);
	} else {
		isc_socket_t *sock;

		dns_dispatch_attach(disp, &client->dispatch);
		sock = dns_dispatch_getsocket(client->dispatch);
		isc_socket_attach(sock, &client->udpsocket);
	}

	INSIST(client->nctls == 0);
	client->nctls++;
	ev = &client->ctlevent;
	isc_task_send(client->task, &ev);

	return (ISC_R_SUCCESS);
}