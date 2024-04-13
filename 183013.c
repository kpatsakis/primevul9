dns_zone_isforced(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_FORCEXFER));
}