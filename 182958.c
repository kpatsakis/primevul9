dns_zone_link(dns_zone_t *zone, dns_zone_t *raw) {
	isc_result_t result;
	dns_zonemgr_t *zmgr;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(zone->zmgr != NULL);
	REQUIRE(zone->task != NULL);
	REQUIRE(zone->loadtask != NULL);
	REQUIRE(zone->raw == NULL);

	REQUIRE(DNS_ZONE_VALID(raw));
	REQUIRE(raw->zmgr == NULL);
	REQUIRE(raw->task == NULL);
	REQUIRE(raw->loadtask == NULL);
	REQUIRE(raw->secure == NULL);

	REQUIRE(zone != raw);

	/*
	 * Lock hierarchy: zmgr, zone, raw.
	 */
	zmgr = zone->zmgr;
	RWLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	LOCK_ZONE(zone);
	LOCK_ZONE(raw);

	result = isc_timer_create(zmgr->timermgr, isc_timertype_inactive,
				  NULL, NULL, zone->task, zone_timer, raw,
				  &raw->timer);
	if (result != ISC_R_SUCCESS)
		goto unlock;

	/*
	 * The timer "holds" a iref.
	 */
	raw->irefs++;
	INSIST(raw->irefs != 0);


	/* dns_zone_attach(raw, &zone->raw); */
	isc_refcount_increment(&raw->erefs);
	zone->raw = raw;

	/* dns_zone_iattach(zone,  &raw->secure); */
	zone_iattach(zone, &raw->secure);

	isc_task_attach(zone->task, &raw->task);
	isc_task_attach(zone->loadtask, &raw->loadtask);

	ISC_LIST_APPEND(zmgr->zones, raw, link);
	raw->zmgr = zmgr;
	zmgr->refs++;

 unlock:
	UNLOCK_ZONE(raw);
	UNLOCK_ZONE(zone);
	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	return (result);
}