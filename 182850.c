dns_zone_clearupdateacl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->update_acl != NULL)
		dns_acl_detach(&zone->update_acl);
	UNLOCK_ZONE(zone);
}