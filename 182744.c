zone_notify(dns_zone_t *zone, isc_time_t *now) {
	dns_dbnode_t *node = NULL;
	dns_db_t *zonedb = NULL;
	dns_dbversion_t *version = NULL;
	dns_name_t *origin = NULL;
	dns_name_t master;
	dns_rdata_ns_t ns;
	dns_rdata_soa_t soa;
	uint32_t serial;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdataset_t nsrdset;
	dns_rdataset_t soardset;
	isc_result_t result;
	unsigned int i;
	isc_sockaddr_t dst;
	bool isqueued;
	dns_notifytype_t notifytype;
	unsigned int flags = 0;
	bool loggednotify = false;
	bool startup;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	startup = !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDNOTIFY);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NEEDNOTIFY);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NEEDSTARTUPNOTIFY);
	notifytype = zone->notifytype;
	DNS_ZONE_TIME_ADD(now, zone->notifydelay, &zone->notifytime);
	UNLOCK_ZONE(zone);

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING) ||
	    ! DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED))
		return;

	if (notifytype == dns_notifytype_no)
		return;

	if (notifytype == dns_notifytype_masteronly &&
	    zone->type != dns_zone_master)
		return;

	origin = &zone->origin;

	/*
	 * If the zone is dialup we are done as we don't want to send
	 * the current soa so as to force a refresh query.
	 */
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DIALNOTIFY))
		flags |= DNS_NOTIFY_NOSOA;

	/*
	 * Record that this was a notify due to starting up.
	 */
	if (startup)
		flags |= DNS_NOTIFY_STARTUP;

	/*
	 * Get SOA RRset.
	 */
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL)
		dns_db_attach(zone->db, &zonedb);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	if (zonedb == NULL)
		return;
	dns_db_currentversion(zonedb, &version);
	result = dns_db_findnode(zonedb, origin, false, &node);
	if (result != ISC_R_SUCCESS)
		goto cleanup1;

	dns_rdataset_init(&soardset);
	result = dns_db_findrdataset(zonedb, node, version, dns_rdatatype_soa,
				     dns_rdatatype_none, 0, &soardset, NULL);
	if (result != ISC_R_SUCCESS)
		goto cleanup2;

	/*
	 * Find serial and master server's name.
	 */
	dns_name_init(&master, NULL);
	result = dns_rdataset_first(&soardset);
	if (result != ISC_R_SUCCESS)
		goto cleanup3;
	dns_rdataset_current(&soardset, &rdata);
	result = dns_rdata_tostruct(&rdata, &soa, NULL);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	dns_rdata_reset(&rdata);
	result = dns_name_dup(&soa.origin, zone->mctx, &master);
	serial = soa.serial;
	dns_rdataset_disassociate(&soardset);
	if (result != ISC_R_SUCCESS)
		goto cleanup3;

	/*
	 * Enqueue notify requests for 'also-notify' servers.
	 */
	LOCK_ZONE(zone);
	for (i = 0; i < zone->notifycnt; i++) {
		dns_tsigkey_t *key = NULL;
		dns_notify_t *notify = NULL;

		if ((zone->notifykeynames != NULL) &&
		    (zone->notifykeynames[i] != NULL)) {
			dns_view_t *view = dns_zone_getview(zone);
			dns_name_t *keyname = zone->notifykeynames[i];
			(void)dns_view_gettsig(view, keyname, &key);
		}

		dst = zone->notify[i];
		if (notify_isqueued(zone, flags, NULL, &dst, key)) {
			if (key != NULL)
				dns_tsigkey_detach(&key);
			continue;
		}

		result = notify_create(zone->mctx, flags, &notify);
		if (result != ISC_R_SUCCESS) {
			if (key != NULL)
				dns_tsigkey_detach(&key);
			continue;
		}

		zone_iattach(zone, &notify->zone);
		notify->dst = dst;

		INSIST(notify->key == NULL);

		if (key != NULL) {
			notify->key = key;
			key = NULL;
		}

		ISC_LIST_APPEND(zone->notifies, notify, link);
		result = notify_send_queue(notify, startup);
		if (result != ISC_R_SUCCESS)
			notify_destroy(notify, true);
		if (!loggednotify) {
			notify_log(zone, ISC_LOG_INFO,
				   "sending notifies (serial %u)",
				   serial);
			loggednotify = true;
		}
	}
	UNLOCK_ZONE(zone);

	if (notifytype == dns_notifytype_explicit)
		goto cleanup3;

	/*
	 * Process NS RRset to generate notifies.
	 */

	dns_rdataset_init(&nsrdset);
	result = dns_db_findrdataset(zonedb, node, version, dns_rdatatype_ns,
				     dns_rdatatype_none, 0, &nsrdset, NULL);
	if (result != ISC_R_SUCCESS)
		goto cleanup3;

	result = dns_rdataset_first(&nsrdset);
	while (result == ISC_R_SUCCESS) {
		dns_notify_t *notify = NULL;

		dns_rdataset_current(&nsrdset, &rdata);
		result = dns_rdata_tostruct(&rdata, &ns, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		dns_rdata_reset(&rdata);
		/*
		 * Don't notify the master server unless explicitly
		 * configured to do so.
		 */
		if (!DNS_ZONE_OPTION(zone, DNS_ZONEOPT_NOTIFYTOSOA) &&
		    dns_name_compare(&master, &ns.name) == 0) {
			result = dns_rdataset_next(&nsrdset);
			continue;
		}

		if (!loggednotify) {
			notify_log(zone, ISC_LOG_INFO,
				   "sending notifies (serial %u)",
				   serial);
			loggednotify = true;
		}

		LOCK_ZONE(zone);
		isqueued = notify_isqueued(zone, flags, &ns.name, NULL, NULL);
		UNLOCK_ZONE(zone);
		if (isqueued) {
			result = dns_rdataset_next(&nsrdset);
			continue;
		}
		result = notify_create(zone->mctx, flags, &notify);
		if (result != ISC_R_SUCCESS)
			continue;
		dns_zone_iattach(zone, &notify->zone);
		result = dns_name_dup(&ns.name, zone->mctx, &notify->ns);
		if (result != ISC_R_SUCCESS) {
			LOCK_ZONE(zone);
			notify_destroy(notify, true);
			UNLOCK_ZONE(zone);
			continue;
		}
		LOCK_ZONE(zone);
		ISC_LIST_APPEND(zone->notifies, notify, link);
		UNLOCK_ZONE(zone);
		notify_find_address(notify);
		result = dns_rdataset_next(&nsrdset);
	}
	dns_rdataset_disassociate(&nsrdset);

 cleanup3:
	if (dns_name_dynamic(&master))
		dns_name_free(&master, zone->mctx);
 cleanup2:
	dns_db_detachnode(zonedb, &node);
 cleanup1:
	dns_db_closeversion(zonedb, &version, false);
	dns_db_detach(&zonedb);
}