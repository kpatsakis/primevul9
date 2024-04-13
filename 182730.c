dns_zone_getdb(dns_zone_t *zone, dns_db_t **dpb) {
	isc_result_t result = ISC_R_SUCCESS;

	REQUIRE(DNS_ZONE_VALID(zone));

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db == NULL)
		result = DNS_R_NOTLOADED;
	else
		dns_db_attach(zone->db, dpb);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

	return (result);
}