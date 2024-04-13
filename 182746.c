dns_zone_setmaxttl(dns_zone_t *zone, dns_ttl_t maxttl) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (maxttl != 0)
		zone->options |= DNS_ZONEOPT_CHECKTTL;
	else
		zone->options &= ~DNS_ZONEOPT_CHECKTTL;
	zone->maxttl = maxttl;
	UNLOCK_ZONE(zone);

	return;
}