dns_zone_setserial(dns_zone_t *zone, uint32_t serial) {
	isc_result_t result = ISC_R_SUCCESS;
	dns_zone_t *dummy = NULL;
	isc_event_t *e = NULL;
	struct ssevent *sse;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);

	if (!inline_secure(zone)) {
		if (!dns_zone_isdynamic(zone, true)) {
			result = DNS_R_NOTDYNAMIC;
			goto failure;
		}
	}

	if (zone->update_disabled) {
		result = DNS_R_FROZEN;
		goto failure;
	}

	e = isc_event_allocate(zone->mctx, zone, DNS_EVENT_SETSERIAL,
			       setserial, zone, sizeof(struct ssevent));
	if (e == NULL) {
		result = ISC_R_NOMEMORY;
		goto failure;
	}

	sse = (struct ssevent *)e;
	sse->serial = serial;

	zone_iattach(zone, &dummy);
	isc_task_send(zone->task, &e);

 failure:
	if (e != NULL)
		isc_event_free(&e);
	UNLOCK_ZONE(zone);
	return (result);
}