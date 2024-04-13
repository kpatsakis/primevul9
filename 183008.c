dns_zone_idetach(dns_zone_t **zonep) {
	dns_zone_t *zone;
	bool free_needed;

	REQUIRE(zonep != NULL && DNS_ZONE_VALID(*zonep));
	zone = *zonep;
	*zonep = NULL;

	LOCK_ZONE(zone);
	INSIST(zone->irefs > 0);
	zone->irefs--;
	free_needed = exit_check(zone);
	UNLOCK_ZONE(zone);
	if (free_needed)
		zone_free(zone);
}