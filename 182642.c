dns_zone_getupdateacl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->update_acl);
}