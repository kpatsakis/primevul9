setnsec3param(isc_task_t *task, isc_event_t *event) {
	const char *me = "setnsec3param";
	bool commit = false;
	isc_result_t result;
	dns_dbversion_t *oldver = NULL, *newver = NULL;
	dns_zone_t *zone;
	dns_db_t *db = NULL;
	dns_dbnode_t *node = NULL;
	dns_rdataset_t prdataset, nrdataset;
	dns_diff_t diff;
	struct np3event *npe = (struct np3event *)event;
	nsec3param_t *np;
	dns_update_log_t log = { update_log_cb, NULL };
	dns_rdata_t rdata;
	bool nseconly;
	bool exists = false;

	UNUSED(task);

	zone = event->ev_arg;
	INSIST(DNS_ZONE_VALID(zone));

	ENTER;

	np = &npe->params;

	dns_rdataset_init(&prdataset);
	dns_rdataset_init(&nrdataset);
	dns_diff_init(zone->mctx, &diff);

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL) {
		dns_db_attach(zone->db, &db);
	}
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	if (db == NULL) {
		goto failure;
	}

	dns_db_currentversion(db, &oldver);
	result = dns_db_newversion(db, &newver);
	if (result != ISC_R_SUCCESS) {
		ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
		dnssec_log(zone, ISC_LOG_ERROR,
			   "setnsec3param:dns_db_newversion -> %s",
			   dns_result_totext(result));
		goto failure;
	}

	CHECK(dns_db_getoriginnode(db, &node));

	/*
	 * Does a private-type record already exist for this chain?
	 */
	result = dns_db_findrdataset(db, node, newver, zone->privatetype,
				     dns_rdatatype_none, 0, &prdataset, NULL);
	if (result == ISC_R_SUCCESS) {
		for (result = dns_rdataset_first(&prdataset);
		     result == ISC_R_SUCCESS;
		     result = dns_rdataset_next(&prdataset))
		{
			dns_rdata_init(&rdata);
			dns_rdataset_current(&prdataset, &rdata);

			if (np->length == rdata.length &&
			    memcmp(rdata.data, np->data, np->length) == 0)
			{
				exists = true;
				break;
			}
		}
	} else if (result != ISC_R_NOTFOUND) {
		INSIST(!dns_rdataset_isassociated(&prdataset));
		goto failure;
	}

	/*
	 * Does the chain already exist?
	 */
	result = dns_db_findrdataset(db, node, newver,
				     dns_rdatatype_nsec3param,
				     dns_rdatatype_none, 0, &nrdataset, NULL);
	if (result == ISC_R_SUCCESS) {
		for (result = dns_rdataset_first(&nrdataset);
		     result == ISC_R_SUCCESS;
		     result = dns_rdataset_next(&nrdataset))
		{
			dns_rdata_init(&rdata);
			dns_rdataset_current(&nrdataset, &rdata);

			if (np->length == (rdata.length + 1) &&
			    memcmp(rdata.data, np->data + 1,
				   np->length - 1) == 0)
			{
				exists = true;
				break;
			}
		}
	} else if (result != ISC_R_NOTFOUND) {
		INSIST(!dns_rdataset_isassociated(&nrdataset));
		goto failure;
	}


	/*
	 * We need to remove any existing NSEC3 chains if the supplied NSEC3
	 * parameters are supposed to replace the current ones or if we are
	 * switching to NSEC.
	 */
	if (!exists && np->replace && (np->length != 0 || np->nsec)) {
		CHECK(dns_nsec3param_deletechains(db, newver, zone,
						  !np->nsec, &diff));
	}

	if (!exists && np->length != 0) {
		/*
		 * We're creating an NSEC3 chain.  Add the private-type record
		 * passed in the event handler's argument to the zone apex.
		 *
		 * If the zone is not currently capable of supporting an NSEC3
		 * chain (due to the DNSKEY RRset at the zone apex not existing
		 * or containing at least one key using an NSEC-only
		 * algorithm), add the INITIAL flag, so these parameters can be
		 * used later when NSEC3 becomes available.
		 */
		dns_rdata_init(&rdata);

		np->data[2] |= DNS_NSEC3FLAG_CREATE;
		result = dns_nsec_nseconly(db, newver, &nseconly);
		if (result == ISC_R_NOTFOUND || nseconly) {
			np->data[2] |= DNS_NSEC3FLAG_INITIAL;
		}

		rdata.length = np->length;
		rdata.data = np->data;
		rdata.type = zone->privatetype;
		rdata.rdclass = zone->rdclass;
		CHECK(update_one_rr(db, newver, &diff, DNS_DIFFOP_ADD,
				    &zone->origin, 0, &rdata));
	}

	/*
	 * If we changed anything in the zone, write changes to journal file
	 * and set commit to true so that resume_addnsec3chain() will be
	 * called below in order to kick off adding/removing relevant NSEC3
	 * records.
	 */
	if (!ISC_LIST_EMPTY(diff.tuples)) {
		CHECK(update_soa_serial(db, newver, &diff, zone->mctx,
					zone->updatemethod));
		result = dns_update_signatures(&log, zone, db,
					       oldver, newver, &diff,
					       zone->sigvalidityinterval);
		if (result != ISC_R_NOTFOUND) {
			CHECK(result);
		}
		CHECK(zone_journal(zone, &diff, NULL, "setnsec3param"));
		commit = true;

		LOCK_ZONE(zone);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_LOADED);
		zone_needdump(zone, 30);
		UNLOCK_ZONE(zone);
	}

 failure:
	if (dns_rdataset_isassociated(&prdataset)) {
		dns_rdataset_disassociate(&prdataset);
	}
	if (dns_rdataset_isassociated(&nrdataset)) {
		dns_rdataset_disassociate(&nrdataset);
	}
	if (node != NULL) {
		dns_db_detachnode(db, &node);
	}
	if (oldver != NULL) {
		dns_db_closeversion(db, &oldver, false);
	}
	if (newver != NULL) {
		dns_db_closeversion(db, &newver, commit);
	}
	if (db != NULL) {
		dns_db_detach(&db);
	}
	if (commit) {
		LOCK_ZONE(zone);
		resume_addnsec3chain(zone);
		UNLOCK_ZONE(zone);
	}
	dns_diff_clear(&diff);
	isc_event_free(&event);
	dns_zone_idetach(&zone);

	INSIST(oldver == NULL);
	INSIST(newver == NULL);
}