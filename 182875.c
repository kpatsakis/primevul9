dns_zone_flush(dns_zone_t *zone) {
	isc_result_t result = ISC_R_SUCCESS;
	bool dumping;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_FLUSH);
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDDUMP) &&
	    zone->masterfile != NULL) {
		result = ISC_R_ALREADYRUNNING;
		dumping = was_dumping(zone);
	} else
		dumping = true;
	UNLOCK_ZONE(zone);
	if (!dumping)
		result = zone_dump(zone, true);	/* Unknown task. */
	return (result);
}