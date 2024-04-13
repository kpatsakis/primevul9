dns_zone_setautomatic(dns_zone_t *zone, bool automatic) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	zone->automatic = automatic;
	UNLOCK_ZONE(zone);
}