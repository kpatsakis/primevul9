dns_zone_clearnotifyacl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->notify_acl != NULL)
		dns_acl_detach(&zone->notify_acl);
	UNLOCK_ZONE(zone);
}