dns_zone_setkeyopt(dns_zone_t *zone, unsigned int keyopt, bool value)
{
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (value)
		zone->keyopts |= keyopt;
	else
		zone->keyopts &= ~keyopt;
	UNLOCK_ZONE(zone);
}