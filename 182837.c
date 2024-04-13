dns_zone_settask(dns_zone_t *zone, isc_task_t *task) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->task != NULL)
		isc_task_detach(&zone->task);
	isc_task_attach(task, &zone->task);
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL)
		dns_db_settask(zone->db, zone->task);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	UNLOCK_ZONE(zone);
}