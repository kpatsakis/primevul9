client_request(isc_task_t *task, isc_event_t *event) {
	ns_client_t *client;
	isc_socketevent_t *sevent;
	isc_result_t result;
	isc_result_t sigresult = ISC_R_SUCCESS;
	isc_buffer_t *buffer;
	isc_buffer_t tbuffer;
	dns_view_t *view;
	dns_rdataset_t *opt;
	dns_name_t *signame;
	bool ra;	/* Recursion available. */
	isc_netaddr_t netaddr;
	int match;
	dns_messageid_t id;
	unsigned int flags;
	bool notimp;
	size_t reqsize;
#ifdef HAVE_DNSTAP
	dns_dtmsgtype_t dtmsgtype;
#endif

	REQUIRE(event != NULL);
	client = event->ev_arg;
	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(task == client->task);

	INSIST(client->recursionquota == NULL);

	INSIST(client->state == (TCP_CLIENT(client) ?
				       NS_CLIENTSTATE_READING :
				       NS_CLIENTSTATE_READY));

	ns_client_requests++;

	if (event->ev_type == ISC_SOCKEVENT_RECVDONE) {
		INSIST(!TCP_CLIENT(client));
		sevent = (isc_socketevent_t *)event;
		REQUIRE(sevent == client->recvevent);
		isc_buffer_init(&tbuffer, sevent->region.base, sevent->n);
		isc_buffer_add(&tbuffer, sevent->n);
		buffer = &tbuffer;
		result = sevent->result;
		if (result == ISC_R_SUCCESS) {
			client->peeraddr = sevent->address;
			client->peeraddr_valid = true;
		}
		if ((sevent->attributes & ISC_SOCKEVENTATTR_DSCP) != 0) {
			ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(90),
			      "received DSCP %d", sevent->dscp);
			if (client->dscp == -1)
				client->dscp = sevent->dscp;
		}
		if ((sevent->attributes & ISC_SOCKEVENTATTR_PKTINFO) != 0) {
			client->attributes |= NS_CLIENTATTR_PKTINFO;
			client->pktinfo = sevent->pktinfo;
		}
		if ((sevent->attributes & ISC_SOCKEVENTATTR_MULTICAST) != 0)
			client->attributes |= NS_CLIENTATTR_MULTICAST;
		client->nrecvs--;
	} else {
		INSIST(TCP_CLIENT(client));
		REQUIRE(event->ev_type == DNS_EVENT_TCPMSG);
		REQUIRE(event->ev_sender == &client->tcpmsg);
		buffer = &client->tcpmsg.buffer;
		result = client->tcpmsg.result;
		INSIST(client->nreads == 1);
		/*
		 * client->peeraddr was set when the connection was accepted.
		 */
		client->nreads--;
	}

	reqsize = isc_buffer_usedlength(buffer);
	/* don't count the length header */
	if (TCP_CLIENT(client))
		reqsize -= 2;

	if (exit_check(client))
		goto cleanup;
	client->state = client->newstate = NS_CLIENTSTATE_WORKING;

	isc_task_getcurrenttimex(task, &client->requesttime);
	client->tnow = client->requesttime;
	client->now = isc_time_seconds(&client->tnow);

	if (result != ISC_R_SUCCESS) {
		if (TCP_CLIENT(client)) {
			ns_client_next(client, result);
		} else {
			if  (result != ISC_R_CANCELED)
				isc_log_write(ns_g_lctx, NS_LOGCATEGORY_CLIENT,
					      NS_LOGMODULE_CLIENT,
					      ISC_LOG_ERROR,
					      "UDP client handler shutting "
					      "down due to fatal receive "
					      "error: %s",
					      isc_result_totext(result));
			isc_task_shutdown(client->task);
		}
		goto cleanup;
	}

	isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);

#if NS_CLIENT_DROPPORT
	if (ns_client_dropport(isc_sockaddr_getport(&client->peeraddr)) ==
	    DROPPORT_REQUEST) {
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(10),
			      "dropped request: suspicious port");
		ns_client_next(client, ISC_R_SUCCESS);
		goto cleanup;
	}
#endif

	ns_client_log(client, NS_LOGCATEGORY_CLIENT,
		      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
		      "%s request",
		      TCP_CLIENT(client) ? "TCP" : "UDP");

	/*
	 * Check the blackhole ACL for UDP only, since TCP is done in
	 * client_newconn.
	 */
	if (!TCP_CLIENT(client)) {
		if (ns_g_server->blackholeacl != NULL &&
		    dns_acl_match(&netaddr, NULL, ns_g_server->blackholeacl,
				  &ns_g_server->aclenv,
				  &match, NULL) == ISC_R_SUCCESS &&
		    match > 0)
		{
			ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
				      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(10),
				      "blackholed UDP datagram");
			ns_client_next(client, ISC_R_SUCCESS);
			goto cleanup;
		}
	}

	/*
	 * Silently drop multicast requests for the present.
	 * XXXMPA revisit this as mDNS spec was published.
	 */
	if ((client->attributes & NS_CLIENTATTR_MULTICAST) != 0) {
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(2),
			      "dropping multicast request");
		ns_client_next(client, DNS_R_REFUSED);
		goto cleanup;
	}

	result = dns_message_peekheader(buffer, &id, &flags);
	if (result != ISC_R_SUCCESS) {
		/*
		 * There isn't enough header to determine whether
		 * this was a request or a response.  Drop it.
		 */
		ns_client_next(client, result);
		goto cleanup;
	}

	/*
	 * The client object handles requests, not responses.
	 * If this is a UDP response, forward it to the dispatcher.
	 * If it's a TCP response, discard it here.
	 */
	if ((flags & DNS_MESSAGEFLAG_QR) != 0) {
		if (TCP_CLIENT(client)) {
			CTRACE("unexpected response");
			ns_client_next(client, DNS_R_FORMERR);
			goto cleanup;
		} else {
			dns_dispatch_importrecv(client->dispatch, event);
			ns_client_next(client, ISC_R_SUCCESS);
			goto cleanup;
		}
	}

	/*
	 * Update some statistics counters.  Don't count responses.
	 */
	if (isc_sockaddr_pf(&client->peeraddr) == PF_INET) {
		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_requestv4);
	} else {
		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_requestv6);
	}
	if (TCP_CLIENT(client)) {
		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_requesttcp);
		switch (isc_sockaddr_pf(&client->peeraddr)) {
		case AF_INET:
			isc_stats_increment(ns_g_server->tcpinstats4,
					    ISC_MIN((int)reqsize / 16, 18));
			break;
		case AF_INET6:
			isc_stats_increment(ns_g_server->tcpinstats6,
					    ISC_MIN((int)reqsize / 16, 18));
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}
	} else {
		switch (isc_sockaddr_pf(&client->peeraddr)) {
		case AF_INET:
			isc_stats_increment(ns_g_server->udpinstats4,
					    ISC_MIN((int)reqsize / 16, 18));
			break;
		case AF_INET6:
			isc_stats_increment(ns_g_server->udpinstats6,
					    ISC_MIN((int)reqsize / 16, 18));
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}
	}

	/*
	 * It's a request.  Parse it.
	 */
	result = dns_message_parse(client->message, buffer, 0);
	if (result != ISC_R_SUCCESS) {
		/*
		 * Parsing the request failed.  Send a response
		 * (typically FORMERR or SERVFAIL).
		 */
		if (result == DNS_R_OPTERR) {
			(void)ns_client_addopt(client, client->message,
					       &client->opt);
		}

		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(1),
			      "message parsing failed: %s",
			      isc_result_totext(result));
		if (result == ISC_R_NOSPACE) {
			result = DNS_R_FORMERR;
		}
		ns_client_error(client, result);
		goto cleanup;
	}

	/*
	 * Pipeline TCP query processing.
	 */
	if (client->message->opcode != dns_opcode_query)
		client->pipelined = false;
	if (TCP_CLIENT(client) && client->pipelined) {
		result = isc_quota_reserve(&ns_g_server->tcpquota);
		if (result == ISC_R_SUCCESS)
			result = ns_client_replace(client);
		if (result != ISC_R_SUCCESS) {
			ns_client_log(client, NS_LOGCATEGORY_CLIENT,
				      NS_LOGMODULE_CLIENT, ISC_LOG_WARNING,
				      "no more TCP clients(read): %s",
				      isc_result_totext(result));
			client->pipelined = false;
		}
	}

	dns_opcodestats_increment(ns_g_server->opcodestats,
				  client->message->opcode);
	switch (client->message->opcode) {
	case dns_opcode_query:
	case dns_opcode_update:
	case dns_opcode_notify:
		notimp = false;
		break;
	case dns_opcode_iquery:
	default:
		notimp = true;
		break;
	}

	client->message->rcode = dns_rcode_noerror;

	/* RFC1123 section 6.1.3.2 */
	if ((client->attributes & NS_CLIENTATTR_MULTICAST) != 0)
		client->message->flags &= ~DNS_MESSAGEFLAG_RD;

	/*
	 * Deal with EDNS.
	 */
	if (ns_g_noedns)
		opt = NULL;
	else
		opt = dns_message_getopt(client->message);

	client->ecs_addrlen = 0;
	client->ecs_scope = 0;

	if (opt != NULL) {
		/*
		 * Are we dropping all EDNS queries?
		 */
		if (ns_g_dropedns) {
			ns_client_next(client, ISC_R_SUCCESS);
			goto cleanup;
		}
		result = process_opt(client, opt);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
	}

	if (client->message->rdclass == 0) {
		if ((client->attributes & NS_CLIENTATTR_WANTCOOKIE) != 0 &&
		    client->message->opcode == dns_opcode_query &&
		    client->message->counts[DNS_SECTION_QUESTION] == 0U)
		{
			result = dns_message_reply(client->message, true);
			if (result != ISC_R_SUCCESS) {
				ns_client_error(client, result);
				return;
			}
			if (notimp)
				client->message->rcode = dns_rcode_notimp;
			ns_client_send(client);
			return;
		}
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(1),
			      "message class could not be determined");
		ns_client_dumpmessage(client,
				      "message class could not be determined");
		ns_client_error(client, notimp ? DNS_R_NOTIMP : DNS_R_FORMERR);
		goto cleanup;
	}

	/*
	 * Determine the destination address.  If the receiving interface is
	 * bound to a specific address, we simply use it regardless of the
	 * address family.  All IPv4 queries should fall into this case.
	 * Otherwise, if this is a TCP query, get the address from the
	 * receiving socket (this needs a system call and can be heavy).
	 * For IPv6 UDP queries, we get this from the pktinfo structure (if
	 * supported).
	 * If all the attempts fail (this can happen due to memory shortage,
	 * etc), we regard this as an error for safety.
	 */
	if ((client->interface->flags & NS_INTERFACEFLAG_ANYADDR) == 0)
		isc_netaddr_fromsockaddr(&client->destaddr,
					 &client->interface->addr);
	else {
		isc_sockaddr_t sockaddr;
		result = ISC_R_FAILURE;

		if (TCP_CLIENT(client))
			result = isc_socket_getsockname(client->tcpsocket,
							&sockaddr);
		if (result == ISC_R_SUCCESS)
			isc_netaddr_fromsockaddr(&client->destaddr, &sockaddr);
		if (result != ISC_R_SUCCESS &&
		    client->interface->addr.type.sa.sa_family == AF_INET6 &&
		    (client->attributes & NS_CLIENTATTR_PKTINFO) != 0) {
			/*
			 * XXXJT technically, we should convert the receiving
			 * interface ID to a proper scope zone ID.  However,
			 * due to the fact there is no standard API for this,
			 * we only handle link-local addresses and use the
			 * interface index as link ID.  Despite the assumption,
			 * it should cover most typical cases.
			 */
			isc_netaddr_fromin6(&client->destaddr,
					    &client->pktinfo.ipi6_addr);
			if (IN6_IS_ADDR_LINKLOCAL(&client->pktinfo.ipi6_addr))
				isc_netaddr_setzone(&client->destaddr,
						client->pktinfo.ipi6_ifindex);
			result = ISC_R_SUCCESS;
		}
		if (result != ISC_R_SUCCESS) {
			UNEXPECTED_ERROR(__FILE__, __LINE__,
					 "failed to get request's "
					 "destination: %s",
					 isc_result_totext(result));
			ns_client_next(client, ISC_R_SUCCESS);
			goto cleanup;
		}
	}

	isc_sockaddr_fromnetaddr(&client->destsockaddr, &client->destaddr, 0);

	/*
	 * Find a view that matches the client's source address.
	 */
	for (view = ISC_LIST_HEAD(ns_g_server->viewlist);
	     view != NULL;
	     view = ISC_LIST_NEXT(view, link)) {
		if (client->message->rdclass == view->rdclass ||
		    client->message->rdclass == dns_rdataclass_any)
		{
			dns_name_t *tsig = NULL;
			isc_netaddr_t *addr = NULL;
			uint8_t *scope = NULL;

			sigresult = dns_message_rechecksig(client->message,
							   view);
			if (sigresult == ISC_R_SUCCESS) {
				dns_tsigkey_t *tsigkey;

				tsigkey = client->message->tsigkey;
				tsig = dns_tsigkey_identity(tsigkey);
			}

			if ((client->attributes & NS_CLIENTATTR_HAVEECS) != 0) {
				addr = &client->ecs_addr;
				scope = &client->ecs_scope;
			}

			if (allowed(&netaddr, tsig, addr, client->ecs_addrlen,
				    scope, view->matchclients) &&
			    allowed(&client->destaddr, tsig, NULL,
				    0, NULL, view->matchdestinations) &&
			    !(view->matchrecursiveonly &&
			    (client->message->flags & DNS_MESSAGEFLAG_RD) == 0))
			{
				dns_view_attach(view, &client->view);
				break;
			}
		}
	}

	if (view == NULL) {
		char classname[DNS_RDATACLASS_FORMATSIZE];

		/*
		 * Do a dummy TSIG verification attempt so that the
		 * response will have a TSIG if the query did, as
		 * required by RFC2845.
		 */
		isc_buffer_t b;
		isc_region_t *r;

		dns_message_resetsig(client->message);

		r = dns_message_getrawmessage(client->message);
		isc_buffer_init(&b, r->base, r->length);
		isc_buffer_add(&b, r->length);
		(void)dns_tsig_verify(&b, client->message, NULL, NULL);

		dns_rdataclass_format(client->message->rdclass, classname,
				      sizeof(classname));
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(1),
			      "no matching view in class '%s'", classname);
		ns_client_dumpmessage(client, "no matching view in class");
		ns_client_error(client, notimp ? DNS_R_NOTIMP : DNS_R_REFUSED);
		goto cleanup;
	}

	ns_client_log(client, NS_LOGCATEGORY_CLIENT,
		      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(5),
		      "using view '%s'", view->name);

	/*
	 * Check for a signature.  We log bad signatures regardless of
	 * whether they ultimately cause the request to be rejected or
	 * not.  We do not log the lack of a signature unless we are
	 * debugging.
	 */
	client->signer = NULL;
	dns_name_init(&client->signername, NULL);
	result = dns_message_signer(client->message, &client->signername);
	if (result != ISC_R_NOTFOUND) {
		signame = NULL;
		if (dns_message_gettsig(client->message, &signame) != NULL) {
			isc_stats_increment(ns_g_server->nsstats,
					    dns_nsstatscounter_tsigin);
		} else {
			isc_stats_increment(ns_g_server->nsstats,
					    dns_nsstatscounter_sig0in);
		}

	}
	if (result == ISC_R_SUCCESS) {
		char namebuf[DNS_NAME_FORMATSIZE];
		dns_name_format(&client->signername, namebuf, sizeof(namebuf));
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "request has valid signature: %s", namebuf);
		client->signer = &client->signername;
	} else if (result == ISC_R_NOTFOUND) {
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "request is not signed");
	} else if (result == DNS_R_NOIDENTITY) {
		ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
			      NS_LOGMODULE_CLIENT, ISC_LOG_DEBUG(3),
			      "request is signed by a nonauthoritative key");
	} else {
		char tsigrcode[64];
		isc_buffer_t b;
		dns_rcode_t status;
		isc_result_t tresult;

		/* There is a signature, but it is bad. */
		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_invalidsig);
		signame = NULL;
		if (dns_message_gettsig(client->message, &signame) != NULL) {
			char namebuf[DNS_NAME_FORMATSIZE];
			char cnamebuf[DNS_NAME_FORMATSIZE];
			dns_name_format(signame, namebuf, sizeof(namebuf));
			status = client->message->tsigstatus;
			isc_buffer_init(&b, tsigrcode, sizeof(tsigrcode) - 1);
			tresult = dns_tsigrcode_totext(status, &b);
			INSIST(tresult == ISC_R_SUCCESS);
			tsigrcode[isc_buffer_usedlength(&b)] = '\0';
			if (client->message->tsigkey->generated) {
				dns_name_format(client->message->tsigkey->creator,
						cnamebuf, sizeof(cnamebuf));
				ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
					      NS_LOGMODULE_CLIENT,
					      ISC_LOG_ERROR,
					      "request has invalid signature: "
					      "TSIG %s (%s): %s (%s)", namebuf,
					      cnamebuf,
					      isc_result_totext(result),
					      tsigrcode);
			} else {
				ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
					      NS_LOGMODULE_CLIENT,
					      ISC_LOG_ERROR,
					      "request has invalid signature: "
					      "TSIG %s: %s (%s)", namebuf,
					      isc_result_totext(result),
					      tsigrcode);
			}
		} else {
			status = client->message->sig0status;
			isc_buffer_init(&b, tsigrcode, sizeof(tsigrcode) - 1);
			tresult = dns_tsigrcode_totext(status, &b);
			INSIST(tresult == ISC_R_SUCCESS);
			tsigrcode[isc_buffer_usedlength(&b)] = '\0';
			ns_client_log(client, DNS_LOGCATEGORY_SECURITY,
				      NS_LOGMODULE_CLIENT, ISC_LOG_ERROR,
				      "request has invalid signature: %s (%s)",
				      isc_result_totext(result), tsigrcode);
		}
		/*
		 * Accept update messages signed by unknown keys so that
		 * update forwarding works transparently through slaves
		 * that don't have all the same keys as the master.
		 */
		if (!(client->message->tsigstatus == dns_tsigerror_badkey &&
		      client->message->opcode == dns_opcode_update)) {
			ns_client_error(client, sigresult);
			goto cleanup;
		}
	}

	/*
	 * Decide whether recursive service is available to this client.
	 * We do this here rather than in the query code so that we can
	 * set the RA bit correctly on all kinds of responses, not just
	 * responses to ordinary queries.  Note if you can't query the
	 * cache there is no point in setting RA.
	 */
	ra = false;
	if (client->view->resolver != NULL &&
	    client->view->recursion == true &&
	    ns_client_checkaclsilent(client, NULL,
				     client->view->recursionacl,
				     true) == ISC_R_SUCCESS &&
	    ns_client_checkaclsilent(client, NULL,
				     client->view->cacheacl,
				     true) == ISC_R_SUCCESS &&
	    ns_client_checkaclsilent(client, &client->destaddr,
				     client->view->recursiononacl,
				     true) == ISC_R_SUCCESS &&
	    ns_client_checkaclsilent(client, &client->destaddr,
				     client->view->cacheonacl,
				     true) == ISC_R_SUCCESS)
		ra = true;

	if (ra == true)
		client->attributes |= NS_CLIENTATTR_RA;

	ns_client_log(client, DNS_LOGCATEGORY_SECURITY, NS_LOGMODULE_CLIENT,
		      ISC_LOG_DEBUG(3), ra ? "recursion available" :
					     "recursion not available");

	/*
	 * Adjust maximum UDP response size for this client.
	 */
	if (client->udpsize > 512) {
		dns_peer_t *peer = NULL;
		uint16_t udpsize = view->maxudp;
		(void) dns_peerlist_peerbyaddr(view->peers, &netaddr, &peer);
		if (peer != NULL)
			dns_peer_getmaxudp(peer, &udpsize);
		if (client->udpsize > udpsize)
			client->udpsize = udpsize;
	}

	/*
	 * Dispatch the request.
	 */
	switch (client->message->opcode) {
	case dns_opcode_query:
		CTRACE("query");
#ifdef HAVE_DNSTAP
		if ((client->message->flags & DNS_MESSAGEFLAG_RD) != 0)
			dtmsgtype = DNS_DTTYPE_CQ;
		else
			dtmsgtype = DNS_DTTYPE_AQ;

		dns_dt_send(view, dtmsgtype, &client->peeraddr,
			    &client->destsockaddr, TCP_CLIENT(client), NULL,
			    &client->requesttime, NULL, buffer);
#endif /* HAVE_DNSTAP */

		ns_query_start(client);
		break;
	case dns_opcode_update:
		CTRACE("update");
		ns_client_settimeout(client, 60);
		ns_update_start(client, sigresult);
		break;
	case dns_opcode_notify:
		CTRACE("notify");
		ns_client_settimeout(client, 60);
		ns_notify_start(client);
		break;
	case dns_opcode_iquery:
		CTRACE("iquery");
		ns_client_error(client, DNS_R_NOTIMP);
		break;
	default:
		CTRACE("unknown opcode");
		ns_client_error(client, DNS_R_NOTIMP);
	}

 cleanup:
	return;
}