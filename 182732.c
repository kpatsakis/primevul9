dns__zone_loadpending(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADPENDING));
}