dns_zone_getxfracl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->xfr_acl);
}