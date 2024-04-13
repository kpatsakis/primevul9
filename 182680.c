dump_done(void *arg, isc_result_t result) {
	const char me[] = "dump_done";
	dns_zone_t *zone = arg;
	dns_zone_t *secure = NULL;
	dns_db_t *db;
	dns_dbversion_t *version;
	bool again = false;
	bool compact = false;
	uint32_t serial;
	isc_result_t tresult;

	REQUIRE(DNS_ZONE_VALID(zone));

	ENTER;

	if (result == ISC_R_SUCCESS && zone->journal != NULL) {
		/*
		 * We don't own these, zone->dctx must stay valid.
		 */
		db = dns_dumpctx_db(zone->dctx);
		version = dns_dumpctx_version(zone->dctx);
		tresult = dns_db_getsoaserial(db, version, &serial);

		/*
		 * Handle lock order inversion.
		 */
 again:
		LOCK_ZONE(zone);
		if (inline_raw(zone)) {
			secure = zone->secure;
			INSIST(secure != zone);
			TRYLOCK_ZONE(result, secure);
			if (result != ISC_R_SUCCESS) {
				UNLOCK_ZONE(zone);
				secure = NULL;
				isc_thread_yield();
				goto again;
			}
		}

		/*
		 * If there is a secure version of this zone
		 * use its serial if it is less than ours.
		 */
		if (tresult == ISC_R_SUCCESS && secure != NULL) {
			uint32_t sserial;
			isc_result_t mresult;

			ZONEDB_LOCK(&secure->dblock, isc_rwlocktype_read);
			if (secure->db != NULL) {
				mresult = dns_db_getsoaserial(zone->secure->db,
							      NULL, &sserial);
				if (mresult == ISC_R_SUCCESS &&
				    isc_serial_lt(sserial, serial))
					serial = sserial;
			}
			ZONEDB_UNLOCK(&secure->dblock, isc_rwlocktype_read);
		}
		if (tresult == ISC_R_SUCCESS && zone->xfr == NULL) {
			dns_db_t *zdb = NULL;
			if (dns_zone_getdb(zone, &zdb) == ISC_R_SUCCESS) {
				zone_journal_compact(zone, zdb, serial);
				dns_db_detach(&zdb);
			}
		} else if (tresult == ISC_R_SUCCESS) {
			compact = true;
			zone->compact_serial = serial;
		}
		if (secure != NULL)
			UNLOCK_ZONE(secure);
		UNLOCK_ZONE(zone);
	}

	LOCK_ZONE(zone);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_DUMPING);
	if (compact)
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDCOMPACT);
	if (result != ISC_R_SUCCESS && result != ISC_R_CANCELED) {
		/*
		 * Try again in a short while.
		 */
		zone_needdump(zone, DNS_DUMP_DELAY);
	} else if (result == ISC_R_SUCCESS &&
		   DNS_ZONE_FLAG(zone, DNS_ZONEFLG_FLUSH) &&
		   DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDDUMP) &&
		   DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED)) {
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NEEDDUMP);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_DUMPING);
		isc_time_settoepoch(&zone->dumptime);
		again = true;
	} else if (result == ISC_R_SUCCESS)
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_FLUSH);

	if (zone->dctx != NULL)
		dns_dumpctx_detach(&zone->dctx);
	zonemgr_putio(&zone->writeio);
	UNLOCK_ZONE(zone);
	if (again)
		(void)zone_dump(zone, false);
	dns_zone_idetach(&zone);
}