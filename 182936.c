dns_zone_replacedb(dns_zone_t *zone, dns_db_t *db, bool dump) {
	isc_result_t result;
	dns_zone_t *secure = NULL;

	REQUIRE(DNS_ZONE_VALID(zone));
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
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_write);
	result = zone_replacedb(zone, db, dump);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_write);
	if (secure != NULL)
		UNLOCK_ZONE(secure);
	UNLOCK_ZONE(zone);
	return (result);
}