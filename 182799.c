dns_zone_setviewcommit(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->prev_view != NULL)
		dns_view_weakdetach(&zone->prev_view);
	UNLOCK_ZONE(zone);
}