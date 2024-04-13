dns_zone_setzeronosoattl(dns_zone_t *zone, bool state) {
	REQUIRE(DNS_ZONE_VALID(zone));
	zone->zero_no_soa_ttl = state;
}