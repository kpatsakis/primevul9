refresh_callback(isc_task_t *task, isc_event_t *event) {
	const char me[] = "refresh_callback";
	dns_requestevent_t *revent = (dns_requestevent_t *)event;
	dns_zone_t *zone;
	dns_message_t *msg = NULL;
	uint32_t soacnt, cnamecnt, soacount, nscount;
	isc_time_t now;
	char master[ISC_SOCKADDR_FORMATSIZE];
	char source[ISC_SOCKADDR_FORMATSIZE];
	dns_rdataset_t *rdataset = NULL;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_soa_t soa;
	isc_result_t result;
	uint32_t serial, oldserial = 0;
	unsigned int j;
	bool do_queue_xfrin = false;

	zone = revent->ev_arg;
	INSIST(DNS_ZONE_VALID(zone));

	UNUSED(task);

	ENTER;

	TIME_NOW(&now);

	LOCK_ZONE(zone);

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING)) {
		isc_event_free(&event);
		dns_request_destroy(&zone->request);
		goto detach;
	}

	/*
	 * if timeout log and next master;
	 */

	isc_sockaddr_format(&zone->masteraddr, master, sizeof(master));
	isc_sockaddr_format(&zone->sourceaddr, source, sizeof(source));

	if (revent->result != ISC_R_SUCCESS) {
		if (revent->result == ISC_R_TIMEDOUT &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NOEDNS)) {
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOEDNS);
			dns_zone_log(zone, ISC_LOG_DEBUG(1),
				     "refresh: timeout retrying without EDNS "
				     "master %s (source %s)", master, source);
			goto same_master;
		}
		if (revent->result == ISC_R_TIMEDOUT &&
		    !dns_request_usedtcp(revent->request)) {
			dns_zone_log(zone, ISC_LOG_INFO,
				     "refresh: retry limit for "
				     "master %s exceeded (source %s)",
				     master, source);
			/* Try with slave with TCP. */
			if ((zone->type == dns_zone_slave ||
			     zone->type == dns_zone_mirror ||
			     zone->type == dns_zone_redirect) &&
			    DNS_ZONE_OPTION(zone, DNS_ZONEOPT_TRYTCPREFRESH))
			{
				if (!dns_zonemgr_unreachable(zone->zmgr,
							     &zone->masteraddr,
							     &zone->sourceaddr,
							     &now))
				{
					DNS_ZONE_SETFLAG(zone,
						     DNS_ZONEFLG_SOABEFOREAXFR);
					goto tcp_transfer;
				}
				dns_zone_log(zone, ISC_LOG_DEBUG(1),
					     "refresh: skipped tcp fallback "
					     "as master %s (source %s) is "
					     "unreachable (cached)",
					      master, source);
			}
		} else
			dns_zone_log(zone, ISC_LOG_INFO,
				     "refresh: failure trying master "
				     "%s (source %s): %s", master, source,
				     dns_result_totext(revent->result));
		goto next_master;
	}

	result = dns_message_create(zone->mctx, DNS_MESSAGE_INTENTPARSE, &msg);
	if (result != ISC_R_SUCCESS)
		goto next_master;
	result = dns_request_getresponse(revent->request, msg, 0);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refresh: failure trying master "
			     "%s (source %s): %s", master, source,
			     dns_result_totext(result));
		goto next_master;
	}

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
				     "refresh: rcode (%.*s) retrying without "
				     "EDNS master %s (source %s)",
				     (int)rb.used, rcode, master, source);
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOEDNS);
			goto same_master;
		}
		if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NOEDNS) &&
		    msg->rcode == dns_rcode_badvers) {
			dns_zone_log(zone, ISC_LOG_DEBUG(1),
				     "refresh: rcode (%.*s) retrying without "
				     "EDNS EXPIRE OPTION master %s (source %s)",
				     (int)rb.used, rcode, master, source);
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOEDNS);
			goto same_master;
		}
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refresh: unexpected rcode (%.*s) from "
			     "master %s (source %s)", (int)rb.used, rcode,
			     master, source);
		/*
		 * Perhaps AXFR/IXFR is allowed even if SOA queries aren't.
		 */
		if (msg->rcode == dns_rcode_refused &&
		    (zone->type == dns_zone_slave ||
		     zone->type == dns_zone_mirror ||
		     zone->type == dns_zone_redirect))
		{
			goto tcp_transfer;
		}
		goto next_master;
	}

	/*
	 * If truncated punt to zone transfer which will query again.
	 */
	if ((msg->flags & DNS_MESSAGEFLAG_TC) != 0) {
		if (zone->type == dns_zone_slave ||
		    zone->type == dns_zone_mirror ||
		    zone->type == dns_zone_redirect)
		{
			dns_zone_log(zone, ISC_LOG_INFO,
				     "refresh: truncated UDP answer, "
				     "initiating TCP zone xfer "
				     "for master %s (source %s)",
				     master, source);
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_SOABEFOREAXFR);
			goto tcp_transfer;
		} else {
			INSIST(zone->type == dns_zone_stub);
			if (dns_request_usedtcp(revent->request)) {
				dns_zone_log(zone, ISC_LOG_INFO,
					     "refresh: truncated TCP response "
					     "from master %s (source %s)",
					     master, source);
				goto next_master;
			}
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_USEVC);
			goto same_master;
		}
	}

	/*
	 * if non-auth log and next master;
	 */
	if ((msg->flags & DNS_MESSAGEFLAG_AA) == 0) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refresh: non-authoritative answer from "
			     "master %s (source %s)", master, source);
		goto next_master;
	}

	cnamecnt = message_count(msg, DNS_SECTION_ANSWER, dns_rdatatype_cname);
	soacnt = message_count(msg, DNS_SECTION_ANSWER, dns_rdatatype_soa);
	nscount = message_count(msg, DNS_SECTION_AUTHORITY, dns_rdatatype_ns);
	soacount = message_count(msg, DNS_SECTION_AUTHORITY,
				 dns_rdatatype_soa);

	/*
	 * There should not be a CNAME record at top of zone.
	 */
	if (cnamecnt != 0) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refresh: CNAME at top of zone "
			     "in master %s (source %s)", master, source);
		goto next_master;
	}

	/*
	 * if referral log and next master;
	 */
	if (soacnt == 0 && soacount == 0 && nscount != 0) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refresh: referral response "
			     "from master %s (source %s)", master, source);
		goto next_master;
	}

	/*
	 * if nodata log and next master;
	 */
	if (soacnt == 0 && (nscount == 0 || soacount != 0)) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refresh: NODATA response "
			     "from master %s (source %s)", master, source);
		goto next_master;
	}

	/*
	 * Only one soa at top of zone.
	 */
	if (soacnt != 1) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refresh: answer SOA count (%d) != 1 "
			     "from master %s (source %s)",
			     soacnt, master, source);
		goto next_master;
	}

	/*
	 * Extract serial
	 */
	rdataset = NULL;
	result = dns_message_findname(msg, DNS_SECTION_ANSWER, &zone->origin,
				      dns_rdatatype_soa, dns_rdatatype_none,
				      NULL, &rdataset);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refresh: unable to get SOA record "
			     "from master %s (source %s)", master, source);
		goto next_master;
	}

	result = dns_rdataset_first(rdataset);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refresh: dns_rdataset_first() failed");
		goto next_master;
	}

	dns_rdataset_current(rdataset, &rdata);
	result = dns_rdata_tostruct(&rdata, &soa, NULL);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	serial = soa.serial;
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED)) {
		unsigned int dbsoacount;
		result = zone_get_from_db(zone, zone->db, NULL, &dbsoacount,
					  &oldserial, NULL, NULL, NULL, NULL,
					  NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		RUNTIME_CHECK(dbsoacount > 0U);
		zone_debuglog(zone, me, 1, "serial: new %u, old %u",
			      serial, oldserial);
	} else
		zone_debuglog(zone, me, 1, "serial: new %u, old not loaded",
			      serial);

	if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED) ||
	    DNS_ZONE_FLAG(zone, DNS_ZONEFLG_FORCEXFER) ||
	    isc_serial_gt(serial, oldserial)) {
		if (dns_zonemgr_unreachable(zone->zmgr, &zone->masteraddr,
					    &zone->sourceaddr, &now))
		{
			dns_zone_log(zone, ISC_LOG_INFO,
				     "refresh: skipping %s as master %s "
				     "(source %s) is unreachable (cached)",
				     (zone->type == dns_zone_slave ||
				      zone->type == dns_zone_mirror ||
				      zone->type == dns_zone_redirect) ?
				     "zone transfer" : "NS query",
				     master, source);
			goto next_master;
		}
 tcp_transfer:
		isc_event_free(&event);
		dns_request_destroy(&zone->request);
		if (zone->type == dns_zone_slave ||
		    zone->type == dns_zone_mirror ||
		    zone->type == dns_zone_redirect)
		{
			do_queue_xfrin = true;
		} else {
			INSIST(zone->type == dns_zone_stub);
			ns_query(zone, rdataset, NULL);
		}
		if (msg != NULL)
			dns_message_destroy(&msg);
	} else if (isc_serial_eq(soa.serial, oldserial)) {
		isc_time_t expiretime;
		uint32_t expire;

		/*
		 * Compute the new expire time based on this response.
		 */
		expire = zone->expire;
		get_edns_expire(zone, msg, &expire);
		DNS_ZONE_TIME_ADD(&now, expire, &expiretime);

		/*
		 * Has the expire time improved?
		 */
		if (isc_time_compare(&expiretime, &zone->expiretime) > 0) {
			zone->expiretime = expiretime;
			if (zone->masterfile != NULL)
				setmodtime(zone, &expiretime);
		}

		DNS_ZONE_JITTER_ADD(&now, zone->refresh, &zone->refreshtime);
		zone->mastersok[zone->curmaster] = true;
		goto next_master;
	} else {
		if (!DNS_ZONE_OPTION(zone, DNS_ZONEOPT_MULTIMASTER))
			dns_zone_log(zone, ISC_LOG_INFO, "serial number (%u) "
				     "received from master %s < ours (%u)",
				     soa.serial, master, oldserial);
		else
			zone_debuglog(zone, me, 1, "ahead");
		zone->mastersok[zone->curmaster] = true;
		goto next_master;
	}
	if (msg != NULL)
		dns_message_destroy(&msg);
	goto detach;

 next_master:
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
	if (zone->curmaster >= zone->masterscnt) {
		bool done = true;
		if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_USEALTXFRSRC) &&
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
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_USEALTXFRSRC);
			zone->curmaster = 0;
			/*
			 * Find the next failed master.
			 */
			while (zone->curmaster < zone->masterscnt &&
			       zone->mastersok[zone->curmaster])
				zone->curmaster++;
			goto requeue;
		}
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_REFRESH);
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDREFRESH)) {
			DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NEEDREFRESH);
			zone->refreshtime = now;
		}
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_USEALTXFRSRC);
		zone_settimer(zone, &now);
		goto detach;
	}

 requeue:
	queue_soa_query(zone);
	goto detach;

 same_master:
	if (msg != NULL)
		dns_message_destroy(&msg);
	isc_event_free(&event);
	dns_request_destroy(&zone->request);
	queue_soa_query(zone);

 detach:
	UNLOCK_ZONE(zone);
	if (do_queue_xfrin)
		queue_xfrin(zone);
	dns_zone_idetach(&zone);
	return;
}