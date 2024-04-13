keydone(isc_task_t *task, isc_event_t *event) {
	const char *me = "keydone";
	bool commit = false;
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_dbversion_t *oldver = NULL, *newver = NULL;
	dns_zone_t *zone;
	dns_db_t *db = NULL;
	dns_dbnode_t *node = NULL;
	dns_rdataset_t rdataset;
	dns_diff_t diff;
	struct keydone *kd = (struct keydone *)event;
	dns_update_log_t log = { update_log_cb, NULL };
	bool clear_pending = false;

	UNUSED(task);

	zone = event->ev_arg;
	INSIST(DNS_ZONE_VALID(zone));

	ENTER;

	dns_rdataset_init(&rdataset);
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
		dnssec_log(zone, ISC_LOG_ERROR,
			   "keydone:dns_db_newversion -> %s",
			   dns_result_totext(result));
		goto failure;
	}

	result = dns_db_getoriginnode(db, &node);
	if (result != ISC_R_SUCCESS) {
		goto failure;
	}

	result = dns_db_findrdataset(db, node, newver, zone->privatetype,
				     dns_rdatatype_none, 0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto failure;
	}
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto failure;
	}

	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset))
	{
		bool found = false;

		dns_rdataset_current(&rdataset, &rdata);

		if (kd->all) {
			if (rdata.length == 5 && rdata.data[0] != 0 &&
			       rdata.data[3] == 0 && rdata.data[4] == 1)
			{
				found = true;
			} else if (rdata.data[0] == 0 &&
				   (rdata.data[2] & PENDINGFLAGS) != 0)
			{
				found = true;
				clear_pending = true;
			}
		} else if (rdata.length == 5 &&
			   memcmp(rdata.data, kd->data, 5) == 0)
		{
			found = true;
		}

		if (found) {
			CHECK(update_one_rr(db, newver, &diff, DNS_DIFFOP_DEL,
					    &zone->origin, rdataset.ttl,
					    &rdata));
		}
		dns_rdata_reset(&rdata);
	}

	if (!ISC_LIST_EMPTY(diff.tuples)) {
		/* Write changes to journal file. */
		CHECK(update_soa_serial(db, newver, &diff, zone->mctx,
					zone->updatemethod));

		result = dns_update_signatures(&log, zone, db,
					       oldver, newver, &diff,
					       zone->sigvalidityinterval);
		if (!clear_pending) {
			CHECK(result);
		}

		CHECK(zone_journal(zone, &diff, NULL, "keydone"));
		commit = true;

		LOCK_ZONE(zone);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_LOADED);
		zone_needdump(zone, 30);
		UNLOCK_ZONE(zone);
	}

 failure:
	if (dns_rdataset_isassociated(&rdataset)) {
		dns_rdataset_disassociate(&rdataset);
	}
	if (db != NULL) {
		if (node != NULL) {
			dns_db_detachnode(db, &node);
		}
		if (oldver != NULL) {
			dns_db_closeversion(db, &oldver, false);
		}
		if (newver != NULL) {
			dns_db_closeversion(db, &newver, commit);
		}
		dns_db_detach(&db);
	}
	dns_diff_clear(&diff);
	isc_event_free(&event);
	dns_zone_idetach(&zone);

	INSIST(oldver == NULL);
	INSIST(newver == NULL);
}