dns_zone_dlzpostload(dns_zone_t *zone, dns_db_t *db)
{
	isc_time_t loadtime;
	isc_result_t result;
	dns_zone_t *secure = NULL;

	TIME_NOW(&loadtime);

	/*
	 * Lock hierarchy: zmgr, zone, raw.
	 */
 again:
	LOCK_ZONE(zone);
	INSIST(zone != zone->raw);
	if (inline_secure(zone))
		LOCK_ZONE(zone->raw);
	else if (inline_raw(zone)) {
		secure = zone->secure;
		TRYLOCK_ZONE(result, secure);
		if (result != ISC_R_SUCCESS) {
			UNLOCK_ZONE(zone);
			secure = NULL;
			isc_thread_yield();
			goto again;
		}
	}
	result = zone_postload(zone, db, loadtime, ISC_R_SUCCESS);
	if (inline_secure(zone))
		UNLOCK_ZONE(zone->raw);
	else if (secure != NULL)
		UNLOCK_ZONE(secure);
	UNLOCK_ZONE(zone);
	return result;
}