receive_secure_db(isc_task_t *task, isc_event_t *event) {
	isc_result_t result;
	dns_zone_t *zone;
	dns_db_t *rawdb, *db = NULL;
	dns_dbnode_t *rawnode = NULL, *node = NULL;
	dns_fixedname_t fname;
	dns_name_t *name;
	dns_dbiterator_t *dbiterator = NULL;
	dns_rdatasetiter_t *rdsit = NULL;
	dns_rdataset_t rdataset;
	dns_dbversion_t *version = NULL;
	isc_time_t loadtime;
	unsigned int oldserial = 0;
	bool have_oldserial = false;
	nsec3paramlist_t nsec3list;
	isc_event_t *setnsec3param_event;
	dns_zone_t *dummy;

	UNUSED(task);

	ISC_LIST_INIT(nsec3list);

	zone = event->ev_arg;
	rawdb = ((struct secure_event *)event)->db;
	isc_event_free(&event);

	name = dns_fixedname_initname(&fname);
	dns_rdataset_init(&rdataset);

	LOCK_ZONE(zone);
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING) || !inline_secure(zone)) {
		result = ISC_R_SHUTTINGDOWN;
		goto failure;
	}

	TIME_NOW(&loadtime);
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL) {
		result = dns_db_getsoaserial(zone->db, NULL, &oldserial);
		if (result == ISC_R_SUCCESS)
			have_oldserial = true;

		/*
		 * assemble nsec3parameters from the old zone, and set a flag
		 * if any are found
		 */
		result = save_nsec3param(zone, &nsec3list);
		if (result != ISC_R_SUCCESS) {
			ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
			goto failure;
		}
	}
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

	result = dns_db_create(zone->mctx, zone->db_argv[0],
			       &zone->origin, dns_dbtype_zone, zone->rdclass,
			       zone->db_argc - 1, zone->db_argv + 1, &db);
	if (result != ISC_R_SUCCESS)
		goto failure;

	result = dns_db_setgluecachestats(db, zone->gluecachestats);
	if (result != ISC_R_SUCCESS && result != ISC_R_NOTIMPLEMENTED) {
		goto failure;
	}

	result = dns_db_newversion(db, &version);
	if (result != ISC_R_SUCCESS)
		goto failure;

	result = dns_db_createiterator(rawdb, 0, &dbiterator);
	if (result != ISC_R_SUCCESS)
		goto failure;

	for (result = dns_dbiterator_first(dbiterator);
	     result == ISC_R_SUCCESS;
	     result = dns_dbiterator_next(dbiterator)) {
		result = dns_dbiterator_current(dbiterator, &rawnode, name);
		if (result != ISC_R_SUCCESS)
			continue;

		result = dns_db_findnode(db, name, true, &node);
		if (result != ISC_R_SUCCESS)
			goto failure;

		result = dns_db_allrdatasets(rawdb, rawnode, NULL, 0, &rdsit);
		if (result != ISC_R_SUCCESS)
			goto failure;

		for (result = dns_rdatasetiter_first(rdsit);
		     result == ISC_R_SUCCESS;
		     result = dns_rdatasetiter_next(rdsit)) {
			dns_rdatasetiter_current(rdsit, &rdataset);
			if (rdataset.type == dns_rdatatype_nsec ||
			    rdataset.type == dns_rdatatype_rrsig ||
			    rdataset.type == dns_rdatatype_nsec3 ||
			    rdataset.type == dns_rdatatype_dnskey ||
			    rdataset.type == dns_rdatatype_nsec3param) {
				dns_rdataset_disassociate(&rdataset);
				continue;
			}
			if (rdataset.type == dns_rdatatype_soa &&
			    have_oldserial) {
				result = checkandaddsoa(db, node, version,
							&rdataset, oldserial);
			} else
				result = dns_db_addrdataset(db, node, version,
							    0, &rdataset, 0,
							    NULL);
			if (result != ISC_R_SUCCESS)
				goto failure;

			dns_rdataset_disassociate(&rdataset);
		}
		dns_rdatasetiter_destroy(&rdsit);
		dns_db_detachnode(rawdb, &rawnode);
		dns_db_detachnode(db, &node);
	}

	/*
	 * Call restore_nsec3param() to create private-type records from
	 * the old nsec3 parameters and insert them into db
	 */
	if (!ISC_LIST_EMPTY(nsec3list)) {
		result = restore_nsec3param(zone, db, version, &nsec3list);
		if (result != ISC_R_SUCCESS) {
			goto failure;
		}
	}

	dns_db_closeversion(db, &version, true);

	/*
	 * Lock hierarchy: zmgr, zone, raw.
	 */
	INSIST(zone != zone->raw);
	LOCK_ZONE(zone->raw);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDNOTIFY);
	result = zone_postload(zone, db, loadtime, ISC_R_SUCCESS);
	zone_needdump(zone, 0); /* XXXMPA */
	UNLOCK_ZONE(zone->raw);

	/*
	 * Process any queued NSEC3PARAM change requests.
	 */
	while (!ISC_LIST_EMPTY(zone->setnsec3param_queue)) {
		setnsec3param_event = ISC_LIST_HEAD(zone->setnsec3param_queue);
		ISC_LIST_UNLINK(zone->setnsec3param_queue, setnsec3param_event,
				ev_link);
		dummy = NULL;
		zone_iattach(zone, &dummy);
		isc_task_send(zone->task, &setnsec3param_event);
	}

 failure:
	UNLOCK_ZONE(zone);
	if (result != ISC_R_SUCCESS)
		dns_zone_log(zone, ISC_LOG_ERROR, "receive_secure_db: %s",
			     dns_result_totext(result));

	while (!ISC_LIST_EMPTY(nsec3list)) {
		nsec3param_t *nsec3p;
		nsec3p = ISC_LIST_HEAD(nsec3list);
		ISC_LIST_UNLINK(nsec3list, nsec3p, link);
		isc_mem_put(zone->mctx, nsec3p, sizeof(nsec3param_t));
	}
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	if (db != NULL) {
		if (node != NULL)
			dns_db_detachnode(db, &node);
		if (version != NULL)
			dns_db_closeversion(db, &version, false);
		dns_db_detach(&db);
	}
	if (rawnode != NULL)
		dns_db_detachnode(rawdb, &rawnode);
	dns_db_detach(&rawdb);
	if (dbiterator != NULL)
		dns_dbiterator_destroy(&dbiterator);
	dns_zone_idetach(&zone);

	INSIST(version == NULL);
}