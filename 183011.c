dns_zone_setadded(dns_zone_t *zone, bool added) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	zone->added = added;
	UNLOCK_ZONE(zone);
}