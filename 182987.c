was_dumping(dns_zone_t *zone) {
	bool dumping;

	REQUIRE(LOCKED_ZONE(zone));

	dumping = DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DUMPING);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_DUMPING);
	if (!dumping) {
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NEEDDUMP);
		isc_time_settoepoch(&zone->dumptime);
	}
	return (dumping);
}