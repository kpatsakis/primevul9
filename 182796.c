zone_loaddone(void *arg, isc_result_t result) {
	static char me[] = "zone_loaddone";
	dns_load_t *load = arg;
	dns_zone_t *zone;
	isc_result_t tresult;
	dns_zone_t *secure = NULL;

	REQUIRE(DNS_LOAD_VALID(load));
	zone = load->zone;

	ENTER;

	tresult = dns_db_endload(load->db, &load->callbacks);
	if (tresult != ISC_R_SUCCESS &&
	    (result == ISC_R_SUCCESS || result == DNS_R_SEENINCLUDE))
		result = tresult;

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
	(void)zone_postload(zone, load->db, load->loadtime, result);
	zonemgr_putio(&zone->readio);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_LOADING);
	zone_idetach(&load->callbacks.zone);
	/*
	 * Leave the zone frozen if the reload fails.
	 */
	if ((result == ISC_R_SUCCESS || result == DNS_R_SEENINCLUDE) &&
	     DNS_ZONE_FLAG(zone, DNS_ZONEFLG_THAW))
		zone->update_disabled = false;
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_THAW);
	if (inline_secure(zone))
		UNLOCK_ZONE(zone->raw);
	else if (secure != NULL)
		UNLOCK_ZONE(secure);
	UNLOCK_ZONE(zone);

	load->magic = 0;
	dns_db_detach(&load->db);
	if (load->zone->lctx != NULL)
		dns_loadctx_detach(&load->zone->lctx);
	dns_zone_idetach(&load->zone);
	isc_mem_putanddetach(&load->mctx, load, sizeof(*load));
}