dns_zone_getnotifyacl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->notify_acl);
}