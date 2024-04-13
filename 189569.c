exit_check(ns_client_t *client) {
	bool destroy_manager = false;
	ns_clientmgr_t *manager = NULL;

	REQUIRE(NS_CLIENT_VALID(client));
	manager = client->manager;

	if (client->state <= client->newstate)
		return (false); /* Business as usual. */

	INSIST(client->newstate < NS_CLIENTSTATE_RECURSING);

	/*
	 * We need to detach from the view early when shutting down
	 * the server to break the following vicious circle:
	 *
	 *  - The resolver will not shut down until the view refcount is zero
	 *  - The view refcount does not go to zero until all clients detach
	 *  - The client does not detach from the view until references is zero
	 *  - references does not go to zero until the resolver has shut down
	 *
	 * Keep the view attached until any outstanding updates complete.
	 */
	if (client->nupdates == 0 &&
	    client->newstate == NS_CLIENTSTATE_FREED && client->view != NULL)
		dns_view_detach(&client->view);

	if (client->state == NS_CLIENTSTATE_WORKING ||
	    client->state == NS_CLIENTSTATE_RECURSING)
	{
		INSIST(client->newstate <= NS_CLIENTSTATE_READING);
		/*
		 * Let the update processing complete.
		 */
		if (client->nupdates > 0)
			return (true);

		/*
		 * We are trying to abort request processing.
		 */
		if (client->nsends > 0) {
			isc_socket_t *sock;
			if (TCP_CLIENT(client))
				sock = client->tcpsocket;
			else
				sock = client->udpsocket;
			isc_socket_cancel(sock, client->task,
					  ISC_SOCKCANCEL_SEND);
		}

		if (! (client->nsends == 0 && client->nrecvs == 0 &&
		       client->references == 0))
		{
			/*
			 * Still waiting for I/O cancel completion.
			 * or lingering references.
			 */
			return (true);
		}

		/*
		 * I/O cancel is complete.  Burn down all state
		 * related to the current request.  Ensure that
		 * the client is no longer on the recursing list.
		 *
		 * We need to check whether the client is still linked,
		 * because it may already have been removed from the
		 * recursing list by ns_client_killoldestquery()
		 */
		if (client->state == NS_CLIENTSTATE_RECURSING) {
			LOCK(&manager->reclock);
			if (ISC_LINK_LINKED(client, rlink))
				ISC_LIST_UNLINK(manager->recursing,
						client, rlink);
			UNLOCK(&manager->reclock);
		}
		ns_client_endrequest(client);

		client->state = NS_CLIENTSTATE_READING;
		INSIST(client->recursionquota == NULL);

		if (NS_CLIENTSTATE_READING == client->newstate) {
			if (!client->pipelined) {
				client_read(client);
				client->newstate = NS_CLIENTSTATE_MAX;
				return (true); /* We're done. */
			} else if (client->mortal) {
				client->newstate = NS_CLIENTSTATE_INACTIVE;
			} else
				return (false);
		}
	}

	if (client->state == NS_CLIENTSTATE_READING) {
		/*
		 * We are trying to abort the current TCP connection,
		 * if any.
		 */
		INSIST(client->recursionquota == NULL);
		INSIST(client->newstate <= NS_CLIENTSTATE_READY);
		if (client->nreads > 0)
			dns_tcpmsg_cancelread(&client->tcpmsg);
		if (client->nreads != 0) {
			/* Still waiting for read cancel completion. */
			return (true);
		}

		if (client->tcpmsg_valid) {
			dns_tcpmsg_invalidate(&client->tcpmsg);
			client->tcpmsg_valid = false;
		}
		if (client->tcpsocket != NULL) {
			CTRACE("closetcp");
			isc_socket_detach(&client->tcpsocket);
		}

		if (client->tcpquota != NULL)
			isc_quota_detach(&client->tcpquota);

		if (client->timerset) {
			(void)isc_timer_reset(client->timer,
					      isc_timertype_inactive,
					      NULL, NULL, true);
			client->timerset = false;
		}

		client->pipelined = false;

		client->peeraddr_valid = false;

		client->state = NS_CLIENTSTATE_READY;
		INSIST(client->recursionquota == NULL);

		/*
		 * Now the client is ready to accept a new TCP connection
		 * or UDP request, but we may have enough clients doing
		 * that already.  Check whether this client needs to remain
		 * active and force it to go inactive if not.
		 *
		 * UDP clients go inactive at this point, but TCP clients
		 * may remain active if we have fewer active TCP client
		 * objects than desired due to an earlier quota exhaustion.
		 */
		if (client->mortal && TCP_CLIENT(client) && !ns_g_clienttest) {
			LOCK(&client->interface->lock);
			if (client->interface->ntcpcurrent <
				    client->interface->ntcptarget)
				client->mortal = false;
			UNLOCK(&client->interface->lock);
		}

		/*
		 * We don't need the client; send it to the inactive
		 * queue for recycling.
		 */
		if (client->mortal) {
			if (client->newstate > NS_CLIENTSTATE_INACTIVE)
				client->newstate = NS_CLIENTSTATE_INACTIVE;
		}

		if (NS_CLIENTSTATE_READY == client->newstate) {
			if (TCP_CLIENT(client)) {
				client_accept(client);
			} else
				client_udprecv(client);
			client->newstate = NS_CLIENTSTATE_MAX;
			return (true);
		}
	}

	if (client->state == NS_CLIENTSTATE_READY) {
		INSIST(client->newstate <= NS_CLIENTSTATE_INACTIVE);

		/*
		 * We are trying to enter the inactive state.
		 */
		if (client->naccepts > 0)
			isc_socket_cancel(client->tcplistener, client->task,
					  ISC_SOCKCANCEL_ACCEPT);

		/* Still waiting for accept cancel completion. */
		if (! (client->naccepts == 0))
			return (true);

		/* Accept cancel is complete. */
		if (client->nrecvs > 0)
			isc_socket_cancel(client->udpsocket, client->task,
					  ISC_SOCKCANCEL_RECV);

		/* Still waiting for recv cancel completion. */
		if (! (client->nrecvs == 0))
			return (true);

		/* Still waiting for control event to be delivered */
		if (client->nctls > 0)
			return (true);

		/* Deactivate the client. */
		if (client->interface)
			ns_interface_detach(&client->interface);

		INSIST(client->naccepts == 0);
		INSIST(client->recursionquota == NULL);
		if (client->tcplistener != NULL)
			isc_socket_detach(&client->tcplistener);

		if (client->udpsocket != NULL)
			isc_socket_detach(&client->udpsocket);

		if (client->dispatch != NULL)
			dns_dispatch_detach(&client->dispatch);

		client->attributes = 0;
		client->mortal = false;

		if (client->keytag != NULL) {
			isc_mem_put(client->mctx, client->keytag,
				    client->keytag_len);
			client->keytag_len = 0;
		}

		/*
		 * Put the client on the inactive list.  If we are aiming for
		 * the "freed" state, it will be removed from the inactive
		 * list shortly, and we need to keep the manager locked until
		 * that has been done, lest the manager decide to reactivate
		 * the dying client inbetween.
		 */
		client->state = NS_CLIENTSTATE_INACTIVE;
		INSIST(client->recursionquota == NULL);

		if (client->state == client->newstate) {
			client->newstate = NS_CLIENTSTATE_MAX;
			if (!ns_g_clienttest && manager != NULL &&
			    !manager->exiting)
				ISC_QUEUE_PUSH(manager->inactive, client,
					       ilink);
			if (client->needshutdown)
				isc_task_shutdown(client->task);
			return (true);
		}
	}

	if (client->state == NS_CLIENTSTATE_INACTIVE) {
		INSIST(client->newstate == NS_CLIENTSTATE_FREED);
		/*
		 * We are trying to free the client.
		 *
		 * When "shuttingdown" is true, either the task has received
		 * its shutdown event or no shutdown event has ever been
		 * set up.  Thus, we have no outstanding shutdown
		 * event at this point.
		 */
		REQUIRE(client->state == NS_CLIENTSTATE_INACTIVE);

		INSIST(client->recursionquota == NULL);
		INSIST(!ISC_QLINK_LINKED(client, ilink));

		if (manager != NULL) {
			LOCK(&manager->listlock);
			ISC_LIST_UNLINK(manager->clients, client, link);
			LOCK(&manager->lock);
			if (manager->exiting &&
			    ISC_LIST_EMPTY(manager->clients))
				destroy_manager = true;
			UNLOCK(&manager->lock);
			UNLOCK(&manager->listlock);
		}

		ns_query_free(client);
		isc_mem_put(client->mctx, client->recvbuf, RECV_BUFFER_SIZE);
		isc_event_free((isc_event_t **)&client->sendevent);
		isc_event_free((isc_event_t **)&client->recvevent);
		isc_timer_detach(&client->timer);
		if (client->delaytimer != NULL)
			isc_timer_detach(&client->delaytimer);

		if (client->tcpbuf != NULL)
			isc_mem_put(client->mctx, client->tcpbuf,
				    TCP_BUFFER_SIZE);
		if (client->opt != NULL) {
			INSIST(dns_rdataset_isassociated(client->opt));
			dns_rdataset_disassociate(client->opt);
			dns_message_puttemprdataset(client->message,
						    &client->opt);
		}
		if (client->keytag != NULL) {
			isc_mem_put(client->mctx, client->keytag,
				    client->keytag_len);
			client->keytag_len = 0;
		}

		dns_message_destroy(&client->message);

		/*
		 * Detaching the task must be done after unlinking from
		 * the manager's lists because the manager accesses
		 * client->task.
		 */
		if (client->task != NULL)
			isc_task_detach(&client->task);

		CTRACE("free");
		client->magic = 0;

		/*
		 * Check that there are no other external references to
		 * the memory context.
		 */
		if (ns_g_clienttest && isc_mem_references(client->mctx) != 1) {
			isc_mem_stats(client->mctx, stderr);
			INSIST(0);
			ISC_UNREACHABLE();
		}

		/*
		 * Destroy the fetchlock mutex that was created in
		 * ns_query_init().
		 */
		DESTROYLOCK(&client->query.fetchlock);

		isc_mem_putanddetach(&client->mctx, client, sizeof(*client));
	}

	if (destroy_manager && manager != NULL)
		clientmgr_destroy(manager);

	return (true);
}