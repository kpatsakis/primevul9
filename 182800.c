notify_send_toaddr(isc_task_t *task, isc_event_t *event) {
	dns_notify_t *notify;
	isc_result_t result;
	dns_message_t *message = NULL;
	isc_netaddr_t dstip;
	dns_tsigkey_t *key = NULL;
	char addrbuf[ISC_SOCKADDR_FORMATSIZE];
	isc_sockaddr_t src;
	unsigned int options, timeout;
	bool have_notifysource = false;
	bool have_notifydscp = false;
	isc_dscp_t dscp = -1;

	notify = event->ev_arg;
	REQUIRE(DNS_NOTIFY_VALID(notify));

	UNUSED(task);

	LOCK_ZONE(notify->zone);

	notify->event = NULL;

	if (DNS_ZONE_FLAG(notify->zone, DNS_ZONEFLG_LOADED) == 0) {
		result = ISC_R_CANCELED;
		goto cleanup;
	}

	if ((event->ev_attributes & ISC_EVENTATTR_CANCELED) != 0 ||
	    DNS_ZONE_FLAG(notify->zone, DNS_ZONEFLG_EXITING) ||
	    notify->zone->view->requestmgr == NULL ||
	    notify->zone->db == NULL) {
		result = ISC_R_CANCELED;
		goto cleanup;
	}

	/*
	 * The raw IPv4 address should also exist.  Don't send to the
	 * mapped form.
	 */
	if (isc_sockaddr_pf(&notify->dst) == PF_INET6 &&
	    IN6_IS_ADDR_V4MAPPED(&notify->dst.type.sin6.sin6_addr)) {
		isc_sockaddr_format(&notify->dst, addrbuf, sizeof(addrbuf));
		notify_log(notify->zone, ISC_LOG_DEBUG(3),
			   "notify: ignoring IPv6 mapped IPV4 address: %s",
			   addrbuf);
		result = ISC_R_CANCELED;
		goto cleanup;
	}

	result = notify_createmessage(notify->zone, notify->flags, &message);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	isc_sockaddr_format(&notify->dst, addrbuf, sizeof(addrbuf));
	if (notify->key != NULL) {
		/* Transfer ownership of key */
		key = notify->key;
		notify->key = NULL;
	} else {
		isc_netaddr_fromsockaddr(&dstip, &notify->dst);
		result = dns_view_getpeertsig(notify->zone->view, &dstip, &key);
		if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND) {
			notify_log(notify->zone, ISC_LOG_ERROR,
				   "NOTIFY to %s not sent. "
				   "Peer TSIG key lookup failure.", addrbuf);
			goto cleanup_message;
		}
	}

	/* XXX: should we log the tsig key too? */
	notify_log(notify->zone, ISC_LOG_DEBUG(3), "sending notify to %s",
		   addrbuf);
	options = 0;
	if (notify->zone->view->peers != NULL) {
		dns_peer_t *peer = NULL;
		bool usetcp = false;
		result = dns_peerlist_peerbyaddr(notify->zone->view->peers,
						 &dstip, &peer);
		if (result == ISC_R_SUCCESS) {
			result = dns_peer_getnotifysource(peer, &src);
			if (result == ISC_R_SUCCESS)
				have_notifysource = true;
			dns_peer_getnotifydscp(peer, &dscp);
			if (dscp != -1)
				have_notifydscp = true;
			result = dns_peer_getforcetcp(peer, &usetcp);
			if (result == ISC_R_SUCCESS && usetcp)
				options |= DNS_FETCHOPT_TCP;
		}
	}
	switch (isc_sockaddr_pf(&notify->dst)) {
	case PF_INET:
		if (!have_notifysource)
			src = notify->zone->notifysrc4;
		if (!have_notifydscp)
			dscp = notify->zone->notifysrc4dscp;
		break;
	case PF_INET6:
		if (!have_notifysource)
			src = notify->zone->notifysrc6;
		if (!have_notifydscp)
			dscp = notify->zone->notifysrc6dscp;
		break;
	default:
		result = ISC_R_NOTIMPLEMENTED;
		goto cleanup_key;
	}
	timeout = 15;
	if (DNS_ZONE_FLAG(notify->zone, DNS_ZONEFLG_DIALNOTIFY))
		timeout = 30;
	result = dns_request_createvia(notify->zone->view->requestmgr,
				       message, &src, &notify->dst, dscp,
				       options, key, timeout * 3, timeout,
				       0, notify->zone->task, notify_done,
				       notify, &notify->request);
	if (result == ISC_R_SUCCESS) {
		if (isc_sockaddr_pf(&notify->dst) == AF_INET) {
			inc_stats(notify->zone,
				  dns_zonestatscounter_notifyoutv4);
		} else {
			inc_stats(notify->zone,
				  dns_zonestatscounter_notifyoutv6);
		}
	}

 cleanup_key:
	if (key != NULL)
		dns_tsigkey_detach(&key);
 cleanup_message:
	dns_message_destroy(&message);
 cleanup:
	UNLOCK_ZONE(notify->zone);
	isc_event_free(&event);
	if (result != ISC_R_SUCCESS)
		notify_destroy(notify, false);
}