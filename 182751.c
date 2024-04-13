dns_zone_clearforwardacl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->forward_acl != NULL)
		dns_acl_detach(&zone->forward_acl);
	UNLOCK_ZONE(zone);
}