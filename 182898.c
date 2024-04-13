soa_query(isc_task_t *task, isc_event_t *event) {
	const char me[] = "soa_query";
	isc_result_t result = ISC_R_FAILURE;
	dns_message_t *message = NULL;
	dns_zone_t *zone = event->ev_arg;
	dns_zone_t *dummy = NULL;
	isc_netaddr_t masterip;
	dns_tsigkey_t *key = NULL;
	uint32_t options;
	bool cancel = true;
	int timeout;
	bool have_xfrsource, have_xfrdscp, reqnsid, reqexpire;
	uint16_t udpsize = SEND_BUFFER_SIZE;
	isc_dscp_t dscp = -1;

	REQUIRE(DNS_ZONE_VALID(zone));

	UNUSED(task);

	ENTER;

	LOCK_ZONE(zone);
	if (((event->ev_attributes & ISC_EVENTATTR_CANCELED) != 0) ||
	    DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING) ||
	    zone->view->requestmgr == NULL) {
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING))
			cancel = false;
		goto cleanup;
	}

 again:
	result = create_query(zone, dns_rdatatype_soa, &message);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	INSIST(zone->masterscnt > 0);
	INSIST(zone->curmaster < zone->masterscnt);

	zone->masteraddr = zone->masters[zone->curmaster];

	isc_netaddr_fromsockaddr(&masterip, &zone->masteraddr);
	/*
	 * First, look for a tsig key in the master statement, then
	 * try for a server key.
	 */
	if ((zone->masterkeynames != NULL) &&
	    (zone->masterkeynames[zone->curmaster] != NULL)) {
		dns_view_t *view = dns_zone_getview(zone);
		dns_name_t *keyname = zone->masterkeynames[zone->curmaster];
		result = dns_view_gettsig(view, keyname, &key);
		if (result != ISC_R_SUCCESS) {
			char namebuf[DNS_NAME_FORMATSIZE];
			dns_name_format(keyname, namebuf, sizeof(namebuf));
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "unable to find key: %s", namebuf);
			goto skip_master;
		}
	}
	if (key == NULL) {
		result = dns_view_getpeertsig(zone->view, &masterip, &key);
		if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND) {
			char addrbuf[ISC_NETADDR_FORMATSIZE];
			isc_netaddr_format(&masterip, addrbuf, sizeof(addrbuf));
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "unable to find TSIG key for %s", addrbuf);
			goto skip_master;
		}
	}

	options = DNS_ZONE_FLAG(zone, DNS_ZONEFLG_USEVC) ?
		  DNS_REQUESTOPT_TCP : 0;
	have_xfrsource = have_xfrdscp = false;
	reqnsid = zone->view->requestnsid;
	reqexpire = zone->requestexpire;
	if (zone->view->peers != NULL) {
		dns_peer_t *peer = NULL;
		bool edns, usetcp;
		result = dns_peerlist_peerbyaddr(zone->view->peers,
						 &masterip, &peer);
		if (result == ISC_R_SUCCESS) {
			result = dns_peer_getsupportedns(peer, &edns);
			if (result == ISC_R_SUCCESS && !edns)
				DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOEDNS);
			result = dns_peer_gettransfersource(peer,
							    &zone->sourceaddr);
			if (result == ISC_R_SUCCESS)
				have_xfrsource = true;
			(void)dns_peer_gettransferdscp(peer, &dscp);
			if (dscp != -1)
				have_xfrdscp = true;
			if (zone->view->resolver != NULL)
				udpsize =
				  dns_resolver_getudpsize(zone->view->resolver);
			(void)dns_peer_getudpsize(peer, &udpsize);
			(void)dns_peer_getrequestnsid(peer, &reqnsid);
			(void)dns_peer_getrequestexpire(peer, &reqexpire);
			result = dns_peer_getforcetcp(peer, &usetcp);
			if (result == ISC_R_SUCCESS && usetcp)
				options |= DNS_REQUESTOPT_TCP;
		}
	}

	switch (isc_sockaddr_pf(&zone->masteraddr)) {
	case PF_INET:
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_USEALTXFRSRC)) {
			if (isc_sockaddr_equal(&zone->altxfrsource4,
					       &zone->xfrsource4))
				goto skip_master;
			zone->sourceaddr = zone->altxfrsource4;
			if (!have_xfrdscp)
				dscp = zone->altxfrsource4dscp;
		} else if (!have_xfrsource) {
			zone->sourceaddr = zone->xfrsource4;
			if (!have_xfrdscp)
				dscp = zone->xfrsource4dscp;
		}
		break;
	case PF_INET6:
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_USEALTXFRSRC)) {
			if (isc_sockaddr_equal(&zone->altxfrsource6,
					       &zone->xfrsource6))
				goto skip_master;
			zone->sourceaddr = zone->altxfrsource6;
			if (!have_xfrdscp)
				dscp = zone->altxfrsource6dscp;
		} else if (!have_xfrsource) {
			zone->sourceaddr = zone->xfrsource6;
			if (!have_xfrdscp)
				dscp = zone->xfrsource6dscp;
		}
		break;
	default:
		result = ISC_R_NOTIMPLEMENTED;
		goto cleanup;
	}

	if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NOEDNS)) {
		result = add_opt(message, udpsize, reqnsid, reqexpire);
		if (result != ISC_R_SUCCESS)
			zone_debuglog(zone, me, 1,
				      "unable to add opt record: %s",
				      dns_result_totext(result));
	}

	zone_iattach(zone, &dummy);
	timeout = 15;
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DIALREFRESH))
		timeout = 30;
	result = dns_request_createvia(zone->view->requestmgr, message,
				       &zone->sourceaddr, &zone->masteraddr,
				       dscp, options, key, timeout * 3,
				       timeout, 0, zone->task,
				       refresh_callback, zone, &zone->request);
	if (result != ISC_R_SUCCESS) {
		zone_idetach(&dummy);
		zone_debuglog(zone, me, 1,
			      "dns_request_createvia4() failed: %s",
			      dns_result_totext(result));
		goto skip_master;
	} else {
		if (isc_sockaddr_pf(&zone->masteraddr) == PF_INET)
			inc_stats(zone, dns_zonestatscounter_soaoutv4);
		else
			inc_stats(zone, dns_zonestatscounter_soaoutv6);
	}
	cancel = false;

 cleanup:
	if (key != NULL)
		dns_tsigkey_detach(&key);
	if (result != ISC_R_SUCCESS)
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_REFRESH);
	if (message != NULL)
		dns_message_destroy(&message);
	if (cancel)
		cancel_refresh(zone);
	isc_event_free(&event);
	UNLOCK_ZONE(zone);
	dns_zone_idetach(&zone);
	return;

 skip_master:
	if (key != NULL)
		dns_tsigkey_detach(&key);
	dns_message_destroy(&message);
	/*
	 * Skip to next failed / untried master.
	 */
	do {
		zone->curmaster++;
	} while (zone->curmaster < zone->masterscnt &&
		 zone->mastersok[zone->curmaster]);
	if (zone->curmaster < zone->masterscnt)
		goto again;
	zone->curmaster = 0;
	goto cleanup;
}