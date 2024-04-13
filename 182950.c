dns_zone_get_parentcatz(const dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (zone->parentcatz);
}