zone_attachdb(dns_zone_t *zone, dns_db_t *db) {
	REQUIRE(zone->db == NULL && db != NULL);

	dns_db_attach(db, &zone->db);
}