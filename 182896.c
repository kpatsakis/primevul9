zone_journal_compact(dns_zone_t *zone, dns_db_t *db, uint32_t serial) {
	isc_result_t result;
	int32_t journalsize;
	dns_dbversion_t *ver = NULL;
	uint64_t dbsize;

	INSIST(LOCKED_ZONE(zone));
	if (inline_raw(zone))
		INSIST(LOCKED_ZONE(zone->secure));

	journalsize = zone->journalsize;
	if (journalsize == -1) {
		journalsize = DNS_JOURNAL_SIZE_MAX;
		dns_db_currentversion(db, &ver);
		result = dns_db_getsize(db, ver, NULL, &dbsize);
		dns_db_closeversion(db, &ver, false);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "zone_journal_compact: "
				     "could not get zone size: %s",
				     isc_result_totext(result));
		} else if (dbsize < DNS_JOURNAL_SIZE_MAX / 2) {
			journalsize = (int32_t)dbsize * 2;
		}
	}
	zone_debuglog(zone, "zone_journal_compact", 1,
		      "target journal size %d", journalsize);
	result = dns_journal_compact(zone->mctx, zone->journal,
				     serial, journalsize);
	switch (result) {
	case ISC_R_SUCCESS:
	case ISC_R_NOSPACE:
	case ISC_R_NOTFOUND:
		dns_zone_log(zone, ISC_LOG_DEBUG(3),
			     "dns_journal_compact: %s",
			     dns_result_totext(result));
		break;
	default:
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "dns_journal_compact failed: %s",
			     dns_result_totext(result));
		break;
	}
}