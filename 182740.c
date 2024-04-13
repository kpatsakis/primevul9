stub_callback(isc_task_t *task, isc_event_t *event) {
	const char me[] = "stub_callback";
	dns_requestevent_t *revent = (dns_requestevent_t *)event;
	dns_stub_t *stub = NULL;
	dns_message_t *msg = NULL;
	dns_zone_t *zone = NULL;
	char master[ISC_SOCKADDR_FORMATSIZE];
	char source[ISC_SOCKADDR_FORMATSIZE];
	uint32_t nscnt, cnamecnt, refresh, retry, expire;
	isc_result_t result;
	isc_time_t now;
	bool exiting = false;
	isc_interval_t i;
	unsigned int j, soacount;

	stub = revent->ev_arg;
	INSIST(DNS_STUB_VALID(stub));

	UNUSED(task);

	zone = stub->zone;

	ENTER;

	TIME_NOW(&now);

	LOCK_ZONE(zone);

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING)) {
		zone_debuglog(zone, me, 1, "exiting");
		exiting = true;
		goto next_master;
	}

	isc_sockaddr_format(&zone->masteraddr, master, sizeof(master));
	isc_sockaddr_format(&zone->sourceaddr, source, sizeof(source));

	if (revent->result != ISC_R_SUCCESS) {
		if (revent->result == ISC_R_TIMEDOUT &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NOEDNS)) {
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOEDNS);
			dns_zone_log(zone, ISC_LOG_DEBUG(1),
				     "refreshing stub: timeout retrying "
				     " without EDNS master %s (source %s)",
				     master, source);
			goto same_master;
		}
		dns_zonemgr_unreachableadd(zone->zmgr, &zone->masteraddr,
					   &zone->sourceaddr, &now);
		dns_zone_log(zone, ISC_LOG_INFO,
			     "could not refresh stub from master %s"
			     " (source %s): %s", master, source,
			     dns_result_totext(revent->result));
		goto next_master;
	}

	result = dns_message_create(zone->mctx, DNS_MESSAGE_INTENTPARSE, &msg);
	if (result != ISC_R_SUCCESS)
		goto next_master;

	result = dns_request_getresponse(revent->request, msg, 0);
	if (result != ISC_R_SUCCESS)
		goto next_master;

	/*
	 * Unexpected rcode.
	 */
	if (msg->rcode != dns_rcode_noerror) {
		char rcode[128];
		isc_buffer_t rb;

		isc_buffer_init(&rb, rcode, sizeof(rcode));
		(void)dns_rcode_totext(msg->rcode, &rb);

		if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NOEDNS) &&
		    (msg->rcode == dns_rcode_servfail ||
		     msg->rcode == dns_rcode_notimp ||
		     msg->rcode == dns_rcode_formerr)) {
			dns_zone_log(zone, ISC_LOG_DEBUG(1),
				     "refreshing stub: rcode (%.*s) retrying "
				     "without EDNS master %s (source %s)",
				     (int)rb.used, rcode, master, source);
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOEDNS);
			goto same_master;
		}

		dns_zone_log(zone, ISC_LOG_INFO,
			     "refreshing stub: "
			     "unexpected rcode (%.*s) from %s (source %s)",
			     (int)rb.used, rcode, master, source);
		goto next_master;
	}

	/*
	 * We need complete messages.
	 */
	if ((msg->flags & DNS_MESSAGEFLAG_TC) != 0) {
		if (dns_request_usedtcp(revent->request)) {
			dns_zone_log(zone, ISC_LOG_INFO,
				     "refreshing stub: truncated TCP "
				     "response from master %s (source %s)",
				     master, source);
			goto next_master;
		}
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_USEVC);
		goto same_master;
	}

	/*
	 * If non-auth log and next master.
	 */
	if ((msg->flags & DNS_MESSAGEFLAG_AA) == 0) {
		dns_zone_log(zone, ISC_LOG_INFO, "refreshing stub: "
			     "non-authoritative answer from "
			     "master %s (source %s)", master, source);
		goto next_master;
	}

	/*
	 * Sanity checks.
	 */
	cnamecnt = message_count(msg, DNS_SECTION_ANSWER, dns_rdatatype_cname);
	nscnt = message_count(msg, DNS_SECTION_ANSWER, dns_rdatatype_ns);

	if (cnamecnt != 0) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refreshing stub: unexpected CNAME response "
			     "from master %s (source %s)", master, source);
		goto next_master;
	}

	if (nscnt == 0) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refreshing stub: no NS records in response "
			     "from master %s (source %s)", master, source);
		goto next_master;
	}

	/*
	 * Save answer.
	 */
	result = save_nsrrset(msg, &zone->origin, stub->db, stub->version);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refreshing stub: unable to save NS records "
			     "from master %s (source %s)", master, source);
		goto next_master;
	}

	/*
	 * Tidy up.
	 */
	dns_db_closeversion(stub->db, &stub->version, true);
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_write);
	if (zone->db == NULL)
		zone_attachdb(zone, stub->db);
	result = zone_get_from_db(zone, zone->db, NULL, &soacount, NULL,
				  &refresh, &retry, &expire, NULL, NULL);
	if (result == ISC_R_SUCCESS && soacount > 0U) {
		zone->refresh = RANGE(refresh, zone->minrefresh,
				      zone->maxrefresh);
		zone->retry = RANGE(retry, zone->minretry, zone->maxretry);
		zone->expire = RANGE(expire, zone->refresh + zone->retry,
				     DNS_MAX_EXPIRE);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_HAVETIMERS);
	}
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_write);
	dns_db_detach(&stub->db);

	dns_message_destroy(&msg);
	isc_event_free(&event);
	dns_request_destroy(&zone->request);

	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_REFRESH);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_LOADED);
	DNS_ZONE_JITTER_ADD(&now, zone->refresh, &zone->refreshtime);
	isc_interval_set(&i, zone->expire, 0);
	DNS_ZONE_TIME_ADD(&now, zone->expire, &zone->expiretime);

	if (zone->masterfile != NULL)
		zone_needdump(zone, 0);

	zone_settimer(zone, &now);
	goto free_stub;

 next_master:
	if (stub->version != NULL)
		dns_db_closeversion(stub->db, &stub->version, false);
	if (stub->db != NULL)
		dns_db_detach(&stub->db);
	if (msg != NULL)
		dns_message_destroy(&msg);
	isc_event_free(&event);
	dns_request_destroy(&zone->request);
	/*
	 * Skip to next failed / untried master.
	 */
	do {
		zone->curmaster++;
	} while (zone->curmaster < zone->masterscnt &&
		 zone->mastersok[zone->curmaster]);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NOEDNS);
	if (exiting || zone->curmaster >= zone->masterscnt) {
		bool done = true;
		if (!exiting &&
		    DNS_ZONE_OPTION(zone, DNS_ZONEOPT_USEALTXFRSRC) &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_USEALTXFRSRC)) {
			/*
			 * Did we get a good answer from all the masters?
			 */
			for (j = 0; j < zone->masterscnt; j++)
				if (zone->mastersok[j] == false) {
					done = false;
					break;
				}
		} else
			done = true;
		if (!done) {
			zone->curmaster = 0;
			/*
			 * Find the next failed master.
			 */
			while (zone->curmaster < zone->masterscnt &&
			       zone->mastersok[zone->curmaster])
				zone->curmaster++;
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_USEALTXFRSRC);
		} else {
			DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_REFRESH);

			zone_settimer(zone, &now);
			goto free_stub;
		}
	}
	queue_soa_query(zone);
	goto free_stub;

 same_master:
	if (msg != NULL)
		dns_message_destroy(&msg);
	isc_event_free(&event);
	dns_request_destroy(&zone->request);
	ns_query(zone, NULL, stub);
	UNLOCK_ZONE(zone);
	goto done;

 free_stub:
	UNLOCK_ZONE(zone);
	stub->magic = 0;
	dns_zone_idetach(&stub->zone);
	INSIST(stub->db == NULL);
	INSIST(stub->version == NULL);
	isc_mem_put(stub->mctx, stub, sizeof(*stub));

 done:
	INSIST(event == NULL);
	return;
}