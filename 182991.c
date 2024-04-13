dns_zone_getqueryacl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->query_acl);
}