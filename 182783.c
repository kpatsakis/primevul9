dns_zone_setdb(dns_zone_t *zone, dns_db_t *db) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(zone->type == dns_zone_staticstub);

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_write);
	REQUIRE(zone->db == NULL);
	dns_db_attach(db, &zone->db);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_write);
}