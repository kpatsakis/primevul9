dns_zone_getqueryonacl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->queryon_acl);
}