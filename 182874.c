inline_raw(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	if (zone->secure != NULL)
		return (true);
	return (false);
}