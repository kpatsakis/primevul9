dns_zone_catz_enable_db(dns_zone_t *zone, dns_db_t *db) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(db != NULL);

	if (zone->catzs != NULL) {
		dns_db_updatenotify_register(db, dns_catz_dbupdate_callback,
					     zone->catzs);
	}
}