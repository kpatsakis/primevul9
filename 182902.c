zone_asyncload(isc_task_t *task, isc_event_t *event) {
	dns_asyncload_t *asl = event->ev_arg;
	dns_zone_t *zone = asl->zone;
	isc_result_t result;

	UNUSED(task);

	REQUIRE(DNS_ZONE_VALID(zone));

	isc_event_free(&event);

	LOCK_ZONE(zone);
	result = zone_load(zone, asl->flags, true);
	if (result != DNS_R_CONTINUE) {
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_LOADPENDING);
	}
	UNLOCK_ZONE(zone);

	/* Inform the zone table we've finished loading */
	if (asl->loaded != NULL)
		(asl->loaded)(asl->loaded_arg, zone, task);

	isc_mem_put(zone->mctx, asl, sizeof (*asl));
	dns_zone_idetach(&zone);
}