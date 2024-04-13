receive_secure_serial(isc_task_t *task, isc_event_t *event) {
	static char me[] = "receive_secure_serial";
	isc_result_t result = ISC_R_SUCCESS;
	dns_journal_t *rjournal = NULL;
	dns_journal_t *sjournal = NULL;
	uint32_t start, end;
	dns_zone_t *zone;
	dns_difftuple_t *tuple = NULL, *soatuple = NULL;
	dns_update_log_t log = { update_log_cb, NULL };
	uint32_t newserial = 0, desired = 0;
	isc_time_t timenow;

	UNUSED(task);

	zone = event->ev_arg;
	end = ((struct secure_event *)event)->serial;

	ENTER;

	LOCK_ZONE(zone);

	/*
	 * If we are already processing a receive secure serial event
	 * for the zone, just queue the new one and exit.
	 */
	if (zone->rss_event != NULL && zone->rss_event != event) {
		ISC_LIST_APPEND(zone->rss_events, event, ev_link);
		UNLOCK_ZONE(zone);
		return;
	}

 nextevent:
	if (zone->rss_event != NULL) {
		INSIST(zone->rss_event == event);
		UNLOCK_ZONE(zone);
	} else {
		zone->rss_event = event;
		dns_diff_init(zone->mctx, &zone->rss_diff);

		/*
		 * zone->db may be NULL, if the load from disk failed.
		 */
		result = ISC_R_SUCCESS;
		ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
		if (zone->db != NULL)
			dns_db_attach(zone->db, &zone->rss_db);
		else
			result = ISC_R_FAILURE;
		ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

		if (result == ISC_R_SUCCESS && zone->raw != NULL)
			dns_zone_attach(zone->raw, &zone->rss_raw);
		else
			result = ISC_R_FAILURE;

		UNLOCK_ZONE(zone);

		CHECK(result);

		/*
		 * We first attempt to sync the raw zone to the secure zone
		 * by using the raw zone's journal, applying all the deltas
		 * from the latest source-serial of the secure zone up to
		 * the current serial number of the raw zone.
		 *
		 * If that fails, then we'll fall back to a direct comparison
		 * between raw and secure zones.
		 */
		CHECK(dns_journal_open(zone->rss_raw->mctx,
				       zone->rss_raw->journal,
				       DNS_JOURNAL_WRITE, &rjournal));

		result = dns_journal_open(zone->mctx, zone->journal,
					  DNS_JOURNAL_READ, &sjournal);
		if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND)
			goto failure;

		if (!dns_journal_get_sourceserial(rjournal, &start)) {
			start = dns_journal_first_serial(rjournal);
			dns_journal_set_sourceserial(rjournal, start);
		}
		if (sjournal != NULL) {
			uint32_t serial;
			/*
			 * We read the secure journal first, if that
			 * exists use its value provided it is greater
			 * that from the raw journal.
			 */
			if (dns_journal_get_sourceserial(sjournal, &serial)) {
				if (isc_serial_gt(serial, start))
					start = serial;
			}
			dns_journal_destroy(&sjournal);
		}

		dns_db_currentversion(zone->rss_db, &zone->rss_oldver);
		CHECK(dns_db_newversion(zone->rss_db, &zone->rss_newver));

		/*
		 * Try to apply diffs from the raw zone's journal to the secure
		 * zone.  If that fails, we recover by syncing up the databases
		 * directly.
		 */
		result = sync_secure_journal(zone, zone->rss_raw, rjournal,
					     start, end, &soatuple,
					     &zone->rss_diff);
		if (result == DNS_R_UNCHANGED)
			goto failure;
		else if (result != ISC_R_SUCCESS)
			CHECK(sync_secure_db(zone, zone->rss_raw, zone->rss_db,
					     zone->rss_oldver, &soatuple,
					     &zone->rss_diff));

		CHECK(dns_diff_apply(&zone->rss_diff, zone->rss_db,
				     zone->rss_newver));

		if (soatuple != NULL) {
			uint32_t oldserial;

			CHECK(dns_db_createsoatuple(zone->rss_db,
						    zone->rss_oldver,
						    zone->rss_diff.mctx,
						    DNS_DIFFOP_DEL, &tuple));
			oldserial = dns_soa_getserial(&tuple->rdata);
			newserial = desired =
				    dns_soa_getserial(&soatuple->rdata);
			if (!isc_serial_gt(newserial, oldserial)) {
				newserial = oldserial + 1;
				if (newserial == 0)
					newserial++;
				dns_soa_setserial(newserial, &soatuple->rdata);
			}
			CHECK(do_one_tuple(&tuple, zone->rss_db,
					   zone->rss_newver, &zone->rss_diff));
			CHECK(do_one_tuple(&soatuple, zone->rss_db,
					   zone->rss_newver, &zone->rss_diff));
		} else
			CHECK(update_soa_serial(zone->rss_db, zone->rss_newver,
						&zone->rss_diff, zone->mctx,
						zone->updatemethod));

	}
	result = dns_update_signaturesinc(&log, zone, zone->rss_db,
					  zone->rss_oldver, zone->rss_newver,
					  &zone->rss_diff,
					  zone->sigvalidityinterval,
					  &zone->rss_state);
	if (result == DNS_R_CONTINUE) {
		if (rjournal != NULL)
			dns_journal_destroy(&rjournal);
		isc_task_send(task, &event);
		return;
	}
	/*
	 * If something went wrong while trying to update the secure zone and
	 * the latter was already signed before, do not apply raw zone deltas
	 * to it as that would break existing DNSSEC signatures.  However, if
	 * the secure zone was not yet signed (e.g. because no signing keys
	 * were created for it), commence applying raw zone deltas to it so
	 * that contents of the raw zone and the secure zone are kept in sync.
	 */
	if (result != ISC_R_SUCCESS && dns_db_issecure(zone->rss_db)) {
		goto failure;
	}

	if (rjournal == NULL)
		CHECK(dns_journal_open(zone->rss_raw->mctx,
				       zone->rss_raw->journal,
				       DNS_JOURNAL_WRITE, &rjournal));
	CHECK(zone_journal(zone, &zone->rss_diff, &end,
			   "receive_secure_serial"));

	dns_journal_set_sourceserial(rjournal, end);
	dns_journal_commit(rjournal);

	LOCK_ZONE(zone);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDNOTIFY);

	zone->sourceserial = end;
	zone->sourceserialset = true;
	zone_needdump(zone, DNS_DUMP_DELAY);

	TIME_NOW(&timenow);
	zone_settimer(zone, &timenow);
	UNLOCK_ZONE(zone);

	dns_db_closeversion(zone->rss_db, &zone->rss_oldver, false);
	dns_db_closeversion(zone->rss_db, &zone->rss_newver, true);

	if (newserial != 0) {
		dns_zone_log(zone, ISC_LOG_INFO, "serial %u (unsigned %u)",
			     newserial, desired);
	}

 failure:
	isc_event_free(&zone->rss_event);
	event = ISC_LIST_HEAD(zone->rss_events);

	if (zone->rss_raw != NULL)
		dns_zone_detach(&zone->rss_raw);
	if (result != ISC_R_SUCCESS)
		dns_zone_log(zone, ISC_LOG_ERROR, "receive_secure_serial: %s",
			     dns_result_totext(result));
	if (tuple != NULL)
		dns_difftuple_free(&tuple);
	if (soatuple != NULL)
		dns_difftuple_free(&soatuple);
	if (zone->rss_db != NULL) {
		if (zone->rss_oldver != NULL)
			dns_db_closeversion(zone->rss_db, &zone->rss_oldver,
					    false);
		if (zone->rss_newver != NULL)
			dns_db_closeversion(zone->rss_db, &zone->rss_newver,
					    false);
		dns_db_detach(&zone->rss_db);
	}
	INSIST(zone->rss_oldver == NULL);
	INSIST(zone->rss_newver == NULL);
	if (rjournal != NULL)
		dns_journal_destroy(&rjournal);
	dns_diff_clear(&zone->rss_diff);

	if (event != NULL) {
		LOCK_ZONE(zone);
		INSIST(zone->irefs > 1);
		zone->irefs--;
		ISC_LIST_UNLINK(zone->rss_events, event, ev_link);
		goto nextevent;
	}
	dns_zone_idetach(&zone);
}