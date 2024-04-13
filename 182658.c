dns_zone_getrcvquerystats(dns_zone_t *zone) {
	if (zone->requeststats_on)
		return (zone->rcvquerystats);
	else
		return (NULL);
}