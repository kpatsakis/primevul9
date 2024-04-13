dns_zone_forcereload(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	if (zone->type == dns_zone_master ||
	    (zone->type == dns_zone_redirect && zone->masters == NULL))
		return;

	LOCK_ZONE(zone);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_FORCEXFER);
	UNLOCK_ZONE(zone);
	dns_zone_refresh(zone);
}