dns_zone_setorigin(dns_zone_t *zone, const dns_name_t *origin) {
	isc_result_t result;
	char namebuf[1024];

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(origin != NULL);

	LOCK_ZONE(zone);
	INSIST(zone != zone->raw);
	if (dns_name_dynamic(&zone->origin)) {
		dns_name_free(&zone->origin, zone->mctx);
		dns_name_init(&zone->origin, NULL);
	}
	result = dns_name_dup(origin, zone->mctx, &zone->origin);

	if (zone->strnamerd != NULL)
		isc_mem_free(zone->mctx, zone->strnamerd);
	if (zone->strname != NULL)
		isc_mem_free(zone->mctx, zone->strname);

	zone_namerd_tostr(zone, namebuf, sizeof namebuf);
	zone->strnamerd = isc_mem_strdup(zone->mctx, namebuf);
	zone_name_tostr(zone, namebuf, sizeof namebuf);
	zone->strname = isc_mem_strdup(zone->mctx, namebuf);

	if (result == ISC_R_SUCCESS && inline_secure(zone))
		result = dns_zone_setorigin(zone->raw, origin);
	UNLOCK_ZONE(zone);
	return (result);
}