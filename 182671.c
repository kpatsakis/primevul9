zone_send_securedb(dns_zone_t *zone, dns_db_t *db) {
	isc_event_t *e;
	dns_db_t *dummy = NULL;
	dns_zone_t *secure = NULL;

	e = isc_event_allocate(zone->secure->mctx, zone,
			       DNS_EVENT_ZONESECUREDB,
			       receive_secure_db, zone->secure,
			       sizeof(struct secure_event));
	if (e == NULL)
		return (ISC_R_NOMEMORY);
	dns_db_attach(db, &dummy);
	((struct secure_event *)e)->db = dummy;
	INSIST(LOCKED_ZONE(zone->secure));
	zone_iattach(zone->secure, &secure);
	isc_task_send(zone->secure->task, &e);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_SENDSECURE);
	return (ISC_R_SUCCESS);
}