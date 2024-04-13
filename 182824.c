dns_zone_load(dns_zone_t *zone, bool newonly) {
	return (zone_load(zone, newonly ? DNS_ZONELOADFLAG_NOSTAT : 0, false));
}