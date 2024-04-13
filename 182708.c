dns_zone_setnotifyacl(dns_zone_t *zone, dns_acl_t *acl) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->notify_acl != NULL)
		dns_acl_detach(&zone->notify_acl);
	dns_acl_attach(acl, &zone->notify_acl);
	UNLOCK_ZONE(zone);
}