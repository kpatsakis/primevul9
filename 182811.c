dns_zone_settype(dns_zone_t *zone, dns_zonetype_t type) {
	char namebuf[1024];

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(type != dns_zone_none);

	/*
	 * Test and set.
	 */
	LOCK_ZONE(zone);
	REQUIRE(zone->type == dns_zone_none || zone->type == type);
	zone->type = type;

	if (zone->strnamerd != NULL)
		isc_mem_free(zone->mctx, zone->strnamerd);

	zone_namerd_tostr(zone, namebuf, sizeof namebuf);
	zone->strnamerd = isc_mem_strdup(zone->mctx, namebuf);
	UNLOCK_ZONE(zone);
}