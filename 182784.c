ns_query(dns_zone_t *zone, dns_rdataset_t *soardataset, dns_stub_t *stub) {
	const char me[] = "ns_query";
	isc_result_t result;
	dns_message_t *message = NULL;
	isc_netaddr_t masterip;
	dns_tsigkey_t *key = NULL;
	dns_dbnode_t *node = NULL;
	int timeout;
	bool have_xfrsource = false, have_xfrdscp = false;
	bool reqnsid;
	uint16_t udpsize = SEND_BUFFER_SIZE;
	isc_dscp_t dscp = -1;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(LOCKED_ZONE(zone));
	REQUIRE((soardataset != NULL && stub == NULL) ||
		(soardataset == NULL && stub != NULL));
	REQUIRE(stub == NULL || DNS_STUB_VALID(stub));

	ENTER;

	if (stub == NULL) {
		stub = isc_mem_get(zone->mctx, sizeof(*stub));
		if (stub == NULL)
			goto cleanup;
		stub->magic = STUB_MAGIC;
		stub->mctx = zone->mctx;
		stub->zone = NULL;
		stub->db = NULL;
		stub->version = NULL;

		/*
		 * Attach so that the zone won't disappear from under us.
		 */
		zone_iattach(zone, &stub->zone);

		/*
		 * If a db exists we will update it, otherwise we create a
		 * new one and attach it to the zone once we have the NS
		 * RRset and glue.
		 */
		ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
		if (zone->db != NULL) {
			dns_db_attach(zone->db, &stub->db);
			ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
		} else {
			ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

			INSIST(zone->db_argc >= 1);
			result = dns_db_create(zone->mctx, zone->db_argv[0],
					       &zone->origin, dns_dbtype_stub,
					       zone->rdclass,
					       zone->db_argc - 1,
					       zone->db_argv + 1,
					       &stub->db);
			if (result != ISC_R_SUCCESS) {
				dns_zone_log(zone, ISC_LOG_ERROR,
					     "refreshing stub: "
					     "could not create "
					     "database: %s",
					     dns_result_totext(result));
				goto cleanup;
			}
			dns_db_settask(stub->db, zone->task);
		}

		result = dns_db_newversion(stub->db, &stub->version);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_INFO, "refreshing stub: "
				     "dns_db_newversion() failed: %s",
				     dns_result_totext(result));
			goto cleanup;
		}

		/*
		 * Update SOA record.
		 */
		result = dns_db_findnode(stub->db, &zone->origin, true,
					 &node);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_INFO, "refreshing stub: "
				     "dns_db_findnode() failed: %s",
				     dns_result_totext(result));
			goto cleanup;
		}

		result = dns_db_addrdataset(stub->db, node, stub->version, 0,
					    soardataset, 0, NULL);
		dns_db_detachnode(stub->db, &node);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_INFO,
				     "refreshing stub: "
				     "dns_db_addrdataset() failed: %s",
				     dns_result_totext(result));
			goto cleanup;
		}
	}

	/*
	 * XXX Optimisation: Create message when zone is setup and reuse.
	 */
	result = create_query(zone, dns_rdatatype_ns, &message);
	INSIST(result == ISC_R_SUCCESS);

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
		}
	}
	if (key == NULL)
		(void)dns_view_getpeertsig(zone->view, &masterip, &key);

	reqnsid = zone->view->requestnsid;
	if (zone->view->peers != NULL) {
		dns_peer_t *peer = NULL;
		bool edns;
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
			result = dns_peer_gettransferdscp(peer, &dscp);
			if (result == ISC_R_SUCCESS && dscp != -1)
				have_xfrdscp = true;
			if (zone->view->resolver != NULL)
				udpsize =
				  dns_resolver_getudpsize(zone->view->resolver);
			(void)dns_peer_getudpsize(peer, &udpsize);
			(void)dns_peer_getrequestnsid(peer, &reqnsid);
		}

	}
	if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NOEDNS)) {
		result = add_opt(message, udpsize, reqnsid, false);
		if (result != ISC_R_SUCCESS)
			zone_debuglog(zone, me, 1,
				      "unable to add opt record: %s",
				      dns_result_totext(result));
	}

	/*
	 * Always use TCP so that we shouldn't truncate in additional section.
	 */
	switch (isc_sockaddr_pf(&zone->masteraddr)) {
	case PF_INET:
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_USEALTXFRSRC)) {
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
		POST(result);
		goto cleanup;
	}
	timeout = 15;
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DIALREFRESH))
		timeout = 30;
	result = dns_request_createvia(zone->view->requestmgr, message,
				       &zone->sourceaddr, &zone->masteraddr,
				       dscp, DNS_REQUESTOPT_TCP, key,
				       timeout * 3, timeout, 0, zone->task,
				       stub_callback, stub, &zone->request);
	if (result != ISC_R_SUCCESS) {
		zone_debuglog(zone, me, 1,
			      "dns_request_createvia() failed: %s",
			      dns_result_totext(result));
		goto cleanup;
	}
	dns_message_destroy(&message);
	goto unlock;

 cleanup:
	cancel_refresh(zone);
	if (stub != NULL) {
		stub->magic = 0;
		if (stub->version != NULL)
			dns_db_closeversion(stub->db, &stub->version,
					    false);
		if (stub->db != NULL)
			dns_db_detach(&stub->db);
		if (stub->zone != NULL)
			zone_idetach(&stub->zone);
		isc_mem_put(stub->mctx, stub, sizeof(*stub));
	}
	if (message != NULL)
		dns_message_destroy(&message);
 unlock:
	if (key != NULL)
		dns_tsigkey_detach(&key);
	return;
}