dns_zone_set_parentcatz(dns_zone_t *zone, dns_catz_zone_t *catz) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(catz != NULL);
	LOCK_ZONE(zone);
	INSIST(zone->parentcatz == NULL || zone->parentcatz == catz);
	zone->parentcatz = catz;
	UNLOCK_ZONE(zone);
}