got_transfer_quota(isc_task_t *task, isc_event_t *event) {
	isc_result_t result = ISC_R_SUCCESS;
	dns_peer_t *peer = NULL;
	char master[ISC_SOCKADDR_FORMATSIZE];
	char source[ISC_SOCKADDR_FORMATSIZE];
	dns_rdatatype_t xfrtype;
	dns_zone_t *zone = event->ev_arg;
	isc_netaddr_t masterip;
	isc_sockaddr_t sourceaddr;
	isc_sockaddr_t masteraddr;
	isc_time_t now;
	const char *soa_before = "";
	isc_dscp_t dscp = -1;
	bool loaded;

	UNUSED(task);

	INSIST(task == zone->task);

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING)) {
		result = ISC_R_CANCELED;
		goto cleanup;
	}

	TIME_NOW(&now);

	isc_sockaddr_format(&zone->masteraddr, master, sizeof(master));
	if (dns_zonemgr_unreachable(zone->zmgr, &zone->masteraddr,
				    &zone->sourceaddr, &now))
	{
		isc_sockaddr_format(&zone->sourceaddr, source, sizeof(source));
		dns_zone_log(zone, ISC_LOG_INFO,
			     "got_transfer_quota: skipping zone transfer as "
			     "master %s (source %s) is unreachable (cached)",
			     master, source);
		result = ISC_R_CANCELED;
		goto cleanup;
	}

	isc_netaddr_fromsockaddr(&masterip, &zone->masteraddr);
	(void)dns_peerlist_peerbyaddr(zone->view->peers, &masterip, &peer);

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_SOABEFOREAXFR))
		soa_before = "SOA before ";
	/*
	 * Decide whether we should request IXFR or AXFR.
	 */
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	loaded = (zone->db != NULL);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

	if (!loaded) {
		dns_zone_log(zone, ISC_LOG_DEBUG(1),
			     "no database exists yet, requesting AXFR of "
			     "initial version from %s", master);
		xfrtype = dns_rdatatype_axfr;
	} else if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_FORCEXFER)) {
		dns_zone_log(zone, ISC_LOG_DEBUG(1),
			     "forced reload, requesting AXFR of "
			     "initial version from %s", master);
		xfrtype = dns_rdatatype_axfr;
	} else if (DNS_ZONE_FLAG(zone, DNS_ZONEFLAG_NOIXFR)) {
		dns_zone_log(zone, ISC_LOG_DEBUG(1),
			     "retrying with AXFR from %s due to "
			     "previous IXFR failure", master);
		xfrtype = dns_rdatatype_axfr;
		LOCK_ZONE(zone);
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLAG_NOIXFR);
		UNLOCK_ZONE(zone);
	} else {
		bool use_ixfr = true;
		if (peer != NULL)
			result = dns_peer_getrequestixfr(peer, &use_ixfr);
		if (peer == NULL || result != ISC_R_SUCCESS)
			use_ixfr = zone->requestixfr;
		if (use_ixfr == false) {
			dns_zone_log(zone, ISC_LOG_DEBUG(1),
				     "IXFR disabled, requesting %sAXFR from %s",
				     soa_before, master);
			if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_SOABEFOREAXFR))
				xfrtype = dns_rdatatype_soa;
			else
				xfrtype = dns_rdatatype_axfr;
		} else {
			dns_zone_log(zone, ISC_LOG_DEBUG(1),
				     "requesting IXFR from %s", master);
			xfrtype = dns_rdatatype_ixfr;
		}
	}

	/*
	 * Determine if we should attempt to sign the request with TSIG.
	 */
	result = ISC_R_NOTFOUND;

	/*
	 * First, look for a tsig key in the master statement, then
	 * try for a server key.
	 */
	if ((zone->masterkeynames != NULL) &&
	    (zone->masterkeynames[zone->curmaster] != NULL)) {
		dns_view_t *view = dns_zone_getview(zone);
		dns_name_t *keyname = zone->masterkeynames[zone->curmaster];
		result = dns_view_gettsig(view, keyname, &zone->tsigkey);
	}
	if (zone->tsigkey == NULL)
		result = dns_view_getpeertsig(zone->view, &masterip,
					      &zone->tsigkey);

	if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "could not get TSIG key for zone transfer: %s",
			     isc_result_totext(result));
	}

	if (zone->masterdscps != NULL)
	    dscp = zone->masterdscps[zone->curmaster];

	LOCK_ZONE(zone);
	masteraddr = zone->masteraddr;
	sourceaddr = zone->sourceaddr;
	switch (isc_sockaddr_pf(&masteraddr)) {
	case PF_INET:
		if (dscp == -1)
			dscp = zone->xfrsource4dscp;
		break;
	case PF_INET6:
		if (dscp == -1)
			dscp = zone->xfrsource6dscp;
		break;
	default:
		INSIST(0);
		ISC_UNREACHABLE();
	}
	UNLOCK_ZONE(zone);
	INSIST(isc_sockaddr_pf(&masteraddr) == isc_sockaddr_pf(&sourceaddr));
	result = dns_xfrin_create(zone, xfrtype, &masteraddr, &sourceaddr,
				  dscp, zone->tsigkey, zone->mctx,
				  zone->zmgr->timermgr, zone->zmgr->socketmgr,
				  zone->task, zone_xfrdone, &zone->xfr);
	if (result == ISC_R_SUCCESS) {
		LOCK_ZONE(zone);
		if (xfrtype == dns_rdatatype_axfr) {
			if (isc_sockaddr_pf(&masteraddr) == PF_INET)
				inc_stats(zone, dns_zonestatscounter_axfrreqv4);
			else
				inc_stats(zone, dns_zonestatscounter_axfrreqv6);
		} else if (xfrtype == dns_rdatatype_ixfr) {
			if (isc_sockaddr_pf(&masteraddr) == PF_INET)
				inc_stats(zone, dns_zonestatscounter_ixfrreqv4);
			else
				inc_stats(zone, dns_zonestatscounter_ixfrreqv6);
		}
		UNLOCK_ZONE(zone);
	}
 cleanup:
	/*
	 * Any failure in this function is handled like a failed
	 * zone transfer.  This ensures that we get removed from
	 * zmgr->xfrin_in_progress.
	 */
	if (result != ISC_R_SUCCESS)
		zone_xfrdone(zone, result);

	isc_event_free(&event);
}