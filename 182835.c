dns_zone_setclass(dns_zone_t *zone, dns_rdataclass_t rdclass) {
	char namebuf[1024];

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(rdclass != dns_rdataclass_none);

	/*
	 * Test and set.
	 */
	LOCK_ZONE(zone);
	INSIST(zone != zone->raw);
	REQUIRE(zone->rdclass == dns_rdataclass_none ||
		zone->rdclass == rdclass);
	zone->rdclass = rdclass;

	if (zone->strnamerd != NULL)
		isc_mem_free(zone->mctx, zone->strnamerd);
	if (zone->strrdclass != NULL)
		isc_mem_free(zone->mctx, zone->strrdclass);

	zone_namerd_tostr(zone, namebuf, sizeof namebuf);
	zone->strnamerd = isc_mem_strdup(zone->mctx, namebuf);
	zone_rdclass_tostr(zone, namebuf, sizeof namebuf);
	zone->strrdclass = isc_mem_strdup(zone->mctx, namebuf);

	if (inline_secure(zone))
		dns_zone_setclass(zone->raw, rdclass);
	UNLOCK_ZONE(zone);
}