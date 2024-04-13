dns_zone_setoption(dns_zone_t *zone, dns_zoneopt_t option,
		   bool value)
{
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (value)
		zone->options |= option;
	else
		zone->options &= ~option;
	UNLOCK_ZONE(zone);
}