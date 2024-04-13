dns_zone_getzeronosoattl(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return (zone->zero_no_soa_ttl);

}