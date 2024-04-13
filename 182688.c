dns_zone_clearqueryonacl(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->queryon_acl != NULL)
		dns_acl_detach(&zone->queryon_acl);
	UNLOCK_ZONE(zone);
}