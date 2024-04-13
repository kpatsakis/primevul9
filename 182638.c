dns_zone_clearqueryacl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->query_acl != NULL)
		dns_acl_detach(&zone->query_acl);
	UNLOCK_ZONE(zone);
}