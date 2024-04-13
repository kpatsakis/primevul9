zone_send_secureserial(dns_zone_t *zone, uint32_t serial) {
	isc_event_t *e;
	dns_zone_t *dummy = NULL;

	e = isc_event_allocate(zone->secure->mctx, zone,
			       DNS_EVENT_ZONESECURESERIAL,
			       receive_secure_serial, zone->secure,
			       sizeof(struct secure_event));
	if (e == NULL)
		return (ISC_R_NOMEMORY);
	((struct secure_event *)e)->serial = serial;
	INSIST(LOCKED_ZONE(zone->secure));
	zone_iattach(zone->secure, &dummy);
	isc_task_send(zone->secure->task, &e);

	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_SENDSECURE);
	return (ISC_R_SUCCESS);
}