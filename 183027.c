dns_zonemgr_managezone(dns_zonemgr_t *zmgr, dns_zone_t *zone) {
	isc_result_t result;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	if (zmgr->zonetasks == NULL)
		return (ISC_R_FAILURE);

	RWLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	LOCK_ZONE(zone);
	REQUIRE(zone->task == NULL);
	REQUIRE(zone->timer == NULL);
	REQUIRE(zone->zmgr == NULL);

	isc_taskpool_gettask(zmgr->zonetasks, &zone->task);
	isc_taskpool_gettask(zmgr->loadtasks, &zone->loadtask);

	/*
	 * Set the task name.  The tag will arbitrarily point to one
	 * of the zones sharing the task (in practice, the one
	 * to be managed last).
	 */
	isc_task_setname(zone->task, "zone", zone);
	isc_task_setname(zone->loadtask, "loadzone", zone);

	result = isc_timer_create(zmgr->timermgr, isc_timertype_inactive,
				  NULL, NULL,
				  zone->task, zone_timer, zone,
				  &zone->timer);

	if (result != ISC_R_SUCCESS)
		goto cleanup_tasks;

	/*
	 * The timer "holds" a iref.
	 */
	zone->irefs++;
	INSIST(zone->irefs != 0);

	ISC_LIST_APPEND(zmgr->zones, zone, link);
	zone->zmgr = zmgr;
	zmgr->refs++;

	goto unlock;

 cleanup_tasks:
	isc_task_detach(&zone->loadtask);
	isc_task_detach(&zone->task);

 unlock:
	UNLOCK_ZONE(zone);
	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	return (result);
}