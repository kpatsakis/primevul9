dns_zone_setview(dns_zone_t *zone, dns_view_t *view) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	dns_zone_setview_helper(zone, view);
	UNLOCK_ZONE(zone);
}