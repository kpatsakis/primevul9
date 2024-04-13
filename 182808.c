zone_detachdb(dns_zone_t *zone) {
	REQUIRE(zone->db != NULL);

	dns_db_detach(&zone->db);
}