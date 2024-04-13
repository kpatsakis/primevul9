dns_zone_dump(dns_zone_t *zone) {
	isc_result_t result = ISC_R_ALREADYRUNNING;
	bool dumping;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	dumping = was_dumping(zone);
	UNLOCK_ZONE(zone);
	if (!dumping)
		result = zone_dump(zone, false);	/* Unknown task. */
	return (result);
}