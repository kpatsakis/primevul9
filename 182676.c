inline_secure(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	if (zone->raw != NULL)
		return (true);
	return (false);
}