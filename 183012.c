setserial(isc_task_t *task, isc_event_t *event) {
	uint32_t oldserial, desired;
	const char *me = "setserial";
	bool commit = false;
	isc_result_t result;
	dns_dbversion_t *oldver = NULL, *newver = NULL;
	dns_zone_t *zone;
	dns_db_t *db = NULL;
	dns_diff_t diff;
	struct ssevent *sse = (struct ssevent *)event;
	dns_update_log_t log = { update_log_cb, NULL };
	dns_difftuple_t *oldtuple = NULL, *newtuple = NULL;

	UNUSED(task);

	zone = event->ev_arg;
	INSIST(DNS_ZONE_VALID(zone));

	ENTER;

	if (zone->update_disabled)
		goto failure;

	desired = sse->serial;

	dns_diff_init(zone->mctx, &diff);

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL)
		dns_db_attach(zone->db, &db);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	if (db == NULL)
		goto failure;

	dns_db_currentversion(db, &oldver);
	result = dns_db_newversion(db, &newver);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "setserial:dns_db_newversion -> %s",
			     dns_result_totext(result));
		goto failure;
	}

	CHECK(dns_db_createsoatuple(db, oldver, diff.mctx,
				    DNS_DIFFOP_DEL, &oldtuple));
	CHECK(dns_difftuple_copy(oldtuple, &newtuple));
	newtuple->op = DNS_DIFFOP_ADD;

	oldserial = dns_soa_getserial(&oldtuple->rdata);
	if (desired == 0U)
		desired = 1;
	if (!isc_serial_gt(desired, oldserial)) {
		if (desired != oldserial)
			dns_zone_log(zone, ISC_LOG_INFO,
				     "setserial: desired serial (%u) "
				     "out of range (%u-%u)", desired,
				     oldserial + 1, (oldserial + 0x7fffffff));
		goto failure;
	}

	dns_soa_setserial(desired, &newtuple->rdata);
	CHECK(do_one_tuple(&oldtuple, db, newver, &diff));
	CHECK(do_one_tuple(&newtuple, db, newver, &diff));
	result = dns_update_signatures(&log, zone, db,
				       oldver, newver, &diff,
				       zone->sigvalidityinterval);
	if (result != ISC_R_NOTFOUND)
		CHECK(result);

	/* Write changes to journal file. */
	CHECK(zone_journal(zone, &diff, NULL, "setserial"));
	commit = true;

	LOCK_ZONE(zone);
	zone_needdump(zone, 30);
	UNLOCK_ZONE(zone);

 failure:
	if (oldtuple != NULL)
		dns_difftuple_free(&oldtuple);
	if (newtuple != NULL)
		dns_difftuple_free(&newtuple);
	if (oldver != NULL)
		dns_db_closeversion(db, &oldver, false);
	if (newver != NULL)
		dns_db_closeversion(db, &newver, commit);
	if (db != NULL)
		dns_db_detach(&db);
	dns_diff_clear(&diff);
	isc_event_free(&event);
	dns_zone_idetach(&zone);

	INSIST(oldver == NULL);
	INSIST(newver == NULL);
}