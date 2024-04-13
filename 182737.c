dns_zone_setviewrevert(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->prev_view != NULL) {
		dns_zone_setview_helper(zone, zone->prev_view);
		dns_view_weakdetach(&zone->prev_view);
	}
	UNLOCK_ZONE(zone);
}