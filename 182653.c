dns_zone_setflag(dns_zone_t *zone, unsigned int flags, bool value) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (value)
		DNS_ZONE_SETFLAG(zone, flags);
	else
		DNS_ZONE_CLRFLAG(zone, flags);
	UNLOCK_ZONE(zone);
}