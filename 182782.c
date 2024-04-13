zone_unload(dns_zone_t *zone) {
	/*
	 * 'zone' locked by caller.
	 */

	REQUIRE(LOCKED_ZONE(zone));

	if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_FLUSH) ||
	    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DUMPING)) {
		if (zone->writeio != NULL)
			zonemgr_cancelio(zone->writeio);

		if (zone->dctx != NULL)
			dns_dumpctx_cancel(zone->dctx);
	}
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_write);
	zone_detachdb(zone);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_write);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_LOADED);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NEEDDUMP);

	if (zone->type == dns_zone_mirror) {
		dns_zone_log(zone, ISC_LOG_INFO,
			     "mirror zone is no longer in use; "
			     "reverting to normal recursion");
	}
}