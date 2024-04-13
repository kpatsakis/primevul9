dns_zone_setsignatures(dns_zone_t *zone, uint32_t signatures) {
	REQUIRE(DNS_ZONE_VALID(zone));

	/*
	 * We treat signatures as a signed value so explicitly
	 * limit its range here.
	 */
	if (signatures > INT32_MAX)
		signatures = INT32_MAX;
	else if (signatures == 0)
		signatures = 1;
	zone->signatures = signatures;
}