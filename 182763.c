dns_zone_setrawdata(dns_zone_t *zone, dns_masterrawheader_t *header) {
	if (zone == NULL)
		return;

	LOCK_ZONE(zone);
	zone_setrawdata(zone, header);
	UNLOCK_ZONE(zone);
}