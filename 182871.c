dns_zone_setxfracl(dns_zone_t *zone, dns_acl_t *acl) {

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->xfr_acl != NULL)
		dns_acl_detach(&zone->xfr_acl);
	dns_acl_attach(acl, &zone->xfr_acl);
	UNLOCK_ZONE(zone);
}