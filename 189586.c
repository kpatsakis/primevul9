client_create(ns_clientmgr_t *manager, ns_client_t **clientp) {
	ns_client_t *client;
	isc_result_t result;
	isc_mem_t *mctx = NULL;

	/*
	 * Caller must be holding the manager lock.
	 *
	 * Note: creating a client does not add the client to the
	 * manager's client list or set the client's manager pointer.
	 * The caller is responsible for that.
	 */

	REQUIRE(clientp != NULL && *clientp == NULL);

	result = get_clientmctx(manager, &mctx);
	if (result != ISC_R_SUCCESS)
		return (result);

	client = isc_mem_get(mctx, sizeof(*client));
	if (client == NULL) {
		isc_mem_detach(&mctx);
		return (ISC_R_NOMEMORY);
	}
	client->mctx = mctx;

	client->task = NULL;
	result = isc_task_create(manager->taskmgr, 0, &client->task);
	if (result != ISC_R_SUCCESS)
		goto cleanup_client;
	isc_task_setname(client->task, "client", client);

	client->timer = NULL;
	result = isc_timer_create(manager->timermgr, isc_timertype_inactive,
				  NULL, NULL, client->task, client_timeout,
				  client, &client->timer);
	if (result != ISC_R_SUCCESS)
		goto cleanup_task;
	client->timerset = false;

	client->delaytimer = NULL;

	client->message = NULL;
	result = dns_message_create(client->mctx, DNS_MESSAGE_INTENTPARSE,
				    &client->message);
	if (result != ISC_R_SUCCESS)
		goto cleanup_timer;

	/* XXXRTH  Hardwired constants */

	client->sendevent = isc_socket_socketevent(client->mctx, client,
						   ISC_SOCKEVENT_SENDDONE,
						   client_senddone, client);
	if (client->sendevent == NULL) {
		result = ISC_R_NOMEMORY;
		goto cleanup_message;
	}

	client->recvbuf = isc_mem_get(client->mctx, RECV_BUFFER_SIZE);
	if  (client->recvbuf == NULL) {
		result = ISC_R_NOMEMORY;
		goto cleanup_sendevent;
	}

	client->recvevent = isc_socket_socketevent(client->mctx, client,
						   ISC_SOCKEVENT_RECVDONE,
						   client_request, client);
	if (client->recvevent == NULL) {
		result = ISC_R_NOMEMORY;
		goto cleanup_recvbuf;
	}

	client->magic = NS_CLIENT_MAGIC;
	client->manager = NULL;
	client->state = NS_CLIENTSTATE_INACTIVE;
	client->newstate = NS_CLIENTSTATE_MAX;
	client->naccepts = 0;
	client->nreads = 0;
	client->nsends = 0;
	client->nrecvs = 0;
	client->nupdates = 0;
	client->nctls = 0;
	client->references = 0;
	client->attributes = 0;
	client->view = NULL;
	client->dispatch = NULL;
	client->udpsocket = NULL;
	client->tcplistener = NULL;
	client->tcpsocket = NULL;
	client->tcpmsg_valid = false;
	client->tcpbuf = NULL;
	client->opt = NULL;
	client->udpsize = 512;
	client->dscp = -1;
	client->extflags = 0;
	client->ednsversion = -1;
	client->next = NULL;
	client->shutdown = NULL;
	client->shutdown_arg = NULL;
	client->signer = NULL;
	dns_name_init(&client->signername, NULL);
	client->mortal = false;
	client->pipelined = false;
	client->tcpquota = NULL;
	client->recursionquota = NULL;
	client->interface = NULL;
	client->peeraddr_valid = false;
	client->ecs_addrlen = 0;
	client->ecs_scope = 0;
#ifdef ALLOW_FILTER_AAAA
	client->filter_aaaa = dns_aaaa_ok;
#endif
	client->needshutdown = ns_g_clienttest;

	ISC_EVENT_INIT(&client->ctlevent, sizeof(client->ctlevent), 0, NULL,
		       NS_EVENT_CLIENTCONTROL, client_start, client, client,
		       NULL, NULL);
	/*
	 * Initialize FORMERR cache to sentinel value that will not match
	 * any actual FORMERR response.
	 */
	isc_sockaddr_any(&client->formerrcache.addr);
	client->formerrcache.time = 0;
	client->formerrcache.id = 0;
	ISC_LINK_INIT(client, link);
	ISC_LINK_INIT(client, rlink);
	ISC_QLINK_INIT(client, ilink);
	client->keytag = NULL;
	client->keytag_len = 0;

	/*
	 * We call the init routines for the various kinds of client here,
	 * after we have created an otherwise valid client, because some
	 * of them call routines that REQUIRE(NS_CLIENT_VALID(client)).
	 */
	result = ns_query_init(client);
	if (result != ISC_R_SUCCESS)
		goto cleanup_recvevent;

	result = isc_task_onshutdown(client->task, client_shutdown, client);
	if (result != ISC_R_SUCCESS)
		goto cleanup_query;

	CTRACE("create");

	*clientp = client;

	return (ISC_R_SUCCESS);

 cleanup_query:
	ns_query_free(client);

 cleanup_recvevent:
	isc_event_free((isc_event_t **)&client->recvevent);

 cleanup_recvbuf:
	isc_mem_put(client->mctx, client->recvbuf, RECV_BUFFER_SIZE);

 cleanup_sendevent:
	isc_event_free((isc_event_t **)&client->sendevent);

	client->magic = 0;

 cleanup_message:
	dns_message_destroy(&client->message);

 cleanup_timer:
	isc_timer_detach(&client->timer);

 cleanup_task:
	isc_task_detach(&client->task);

 cleanup_client:
	isc_mem_putanddetach(&client->mctx, client, sizeof(*client));

	return (result);
}