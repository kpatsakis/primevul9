dns_zone_nameonly(dns_zone_t *zone, char *buf, size_t length) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(buf != NULL);
	zone_name_tostr(zone, buf, length);
}