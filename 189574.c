client_newconn(isc_task_t *task, isc_event_t *event) {
	ns_client_t *client = event->ev_arg;
	isc_socket_newconnev_t *nevent = (isc_socket_newconnev_t *)event;
	isc_result_t result;

	REQUIRE(event->ev_type == ISC_SOCKEVENT_NEWCONN);
	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(client->task == task);

	UNUSED(task);

	INSIST(client->state == NS_CLIENTSTATE_READY);

	INSIST(client->naccepts == 1);
	client->naccepts--;

	LOCK(&client->interface->lock);
	INSIST(client->interface->ntcpcurrent > 0);
	client->interface->ntcpcurrent--;
	UNLOCK(&client->interface->lock);

	/*
	 * We must take ownership of the new socket before the exit
	 * check to make sure it gets destroyed if we decide to exit.
	 */
	if (nevent->result == ISC_R_SUCCESS) {
		client->tcpsocket = nevent->newsocket;
		isc_socket_setname(client->tcpsocket, "client-tcp", NULL);
		client->state = NS_CLIENTSTATE_READING;
		INSIST(client->recursionquota == NULL);

		(void)isc_socket_getpeername(client->tcpsocket,
					     &client->peeraddr);
		client->peeraddr_valid = true;
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			   NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			   "new TCP connection");
	} else {
		/*
		 * XXXRTH  What should we do?  We're trying to accept but
		 *	   it didn't work.  If we just give up, then TCP
		 *	   service may eventually stop.
		 *
		 *	   For now, we just go idle.
		 *
		 *	   Going idle is probably the right thing if the
		 *	   I/O was canceled.
		 */
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "accept failed: %s",
			      isc_result_totext(nevent->result));
	}

	if (exit_check(client))
		goto freeevent;

	if (nevent->result == ISC_R_SUCCESS) {
		int match;
		isc_netaddr_t netaddr;

		isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);

		if (ns_g_server->blackholeacl != NULL &&
		    dns_acl_match(&netaddr, NULL,
				  ns_g_server->blackholeacl,
				  &ns_g_server->aclenv,
				  &match, NULL) == ISC_R_SUCCESS &&
		    match > 0)
		{
			ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
				      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(10),
				      "blackholed connection attempt");
			client->newstate = NS_CLIENTSTATE_READY;
			(void)exit_check(client);
			goto freeevent;
		}

		INSIST(client->tcpmsg_valid == false);
		dns_tcpmsg_init(client->mctx, client->tcpsocket,
				&client->tcpmsg);
		client->tcpmsg_valid = true;

		/*
		 * Let a new client take our place immediately, before
		 * we wait for a request packet.  If we don't,
		 * telnetting to port 53 (once per CPU) will
		 * deny service to legitimate TCP clients.
		 */
		client->pipelined = false;
		result = isc_quota_attach(&ns_g_server->tcpquota,
					  &client->tcpquota);
		if (result == ISC_R_SUCCESS)
			result = ns_client_replace(client);
		if (result != ISC_R_SUCCESS) {
			ns_client_log(client, NS_LOGCATEGORY_CLIENT,
				      NS_LOGMODULE_CLIENT, ISC_LOG_WARNING,
				      "no more TCP clients(accept): %s",
				      isc_result_totext(result));
		} else if (ns_g_server->keepresporder == NULL ||
			   !allowed(&netaddr, NULL, NULL, 0, NULL,
				    ns_g_server->keepresporder)) {
			client->pipelined = true;
		}

		client_read(client);
	}

 freeevent:
	isc_event_free(&event);
}