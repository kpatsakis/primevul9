dns_zone_clearxfracl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->xfr_acl != NULL)
		dns_acl_detach(&zone->xfr_acl);
	UNLOCK_ZONE(zone);
}