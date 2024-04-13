dns_zone_synckeyzone(dns_zone_t *zone) {
	isc_result_t result;
	dns_db_t *db = NULL;

	if (zone->type != dns_zone_key) {
		return (DNS_R_BADZONE);
	}

	CHECK(dns_zone_getdb(zone, &db));

	LOCK_ZONE(zone);
	result = sync_keyzone(zone, db);
	UNLOCK_ZONE(zone);

 failure:
	if (db != NULL) {
		dns_db_detach(&db);
	}
	return (result);
}