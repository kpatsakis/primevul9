dns_zone_setchecknames(dns_zone_t *zone, dns_severity_t severity) {
	REQUIRE(DNS_ZONE_VALID(zone));

	zone->check_names = severity;
}