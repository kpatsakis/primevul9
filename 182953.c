dns_zone_getsigresigninginterval(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->sigresigninginterval);
}