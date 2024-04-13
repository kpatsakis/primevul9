dns_zone_detach(dns_zone_t **zonep) {
	REQUIRE(zonep != NULL && DNS_ZONE_VALID(*zonep));
	dns_zone_t *zone = *zonep;
	*zonep = NULL;

	bool free_now = false;
	dns_zone_t *raw = NULL;
	dns_zone_t *secure = NULL;
	if (isc_refcount_decrement(&zone->erefs) == 1) {
		isc_refcount_destroy(&zone->erefs);

		LOCK_ZONE(zone);
		INSIST(zone != zone->raw);
		/*
		 * We just detached the last external reference.
		 */
		if (zone->task != NULL) {
			/*
			 * This zone is being managed.	Post
			 * its control event and let it clean
			 * up synchronously in the context of
			 * its task.
			 */
			isc_event_t *ev = &zone->ctlevent;
			isc_task_send(zone->task, &ev);
		} else {
			/*
			 * This zone is not being managed; it has
			 * no task and can have no outstanding
			 * events.  Free it immediately.
			 */
			/*
			 * Unmanaged zones should not have non-null views;
			 * we have no way of detaching from the view here
			 * without causing deadlock because this code is called
			 * with the view already locked.
			 */
			INSIST(zone->view == NULL);
			free_now = true;
			raw = zone->raw;
			zone->raw = NULL;
			secure = zone->secure;
			zone->secure = NULL;
		}
		UNLOCK_ZONE(zone);
	}
	if (free_now) {
		if (raw != NULL) {
			dns_zone_detach(&raw);
		}
		if (secure != NULL) {
			dns_zone_idetach(&secure);
		}
		zone_free(zone);
	}
}