queue_soa_query(dns_zone_t *zone) {
	const char me[] = "queue_soa_query";
	isc_event_t *e;
	dns_zone_t *dummy = NULL;
	isc_result_t result;

	ENTER;
	/*
	 * Locked by caller
	 */
	REQUIRE(LOCKED_ZONE(zone));

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING)) {
		cancel_refresh(zone);
		return;
	}

	e = isc_event_allocate(zone->mctx, NULL, DNS_EVENT_ZONE,
			       soa_query, zone, sizeof(isc_event_t));
	if (e == NULL) {
		cancel_refresh(zone);
		return;
	}

	/*
	 * Attach so that we won't clean up
	 * until the event is delivered.
	 */
	zone_iattach(zone, &dummy);

	e->ev_arg = zone;
	e->ev_sender = NULL;
	result = isc_ratelimiter_enqueue(zone->zmgr->refreshrl, zone->task, &e);
	if (result != ISC_R_SUCCESS) {
		zone_idetach(&dummy);
		isc_event_free(&e);
		cancel_refresh(zone);
	}
}