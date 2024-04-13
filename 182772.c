dns_zone_name(dns_zone_t *zone, char *buf, size_t length) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(buf != NULL);
	zone_namerd_tostr(zone, buf, length);
}