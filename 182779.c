dns_zone_getserial(dns_zone_t *zone, uint32_t *serialp) {
	isc_result_t result;
	unsigned int soacount;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(serialp != NULL);

	LOCK_ZONE(zone);
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL) {
		result = zone_get_from_db(zone, zone->db, NULL, &soacount,
					  serialp, NULL, NULL, NULL, NULL,
					  NULL);
		if (result == ISC_R_SUCCESS && soacount == 0)
			result = ISC_R_FAILURE;
	} else
		result = DNS_R_NOTLOADED;
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	UNLOCK_ZONE(zone);

	return (result);
}