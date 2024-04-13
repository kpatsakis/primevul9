zone_shutdown(isc_task_t *task, isc_event_t *event) {
	dns_zone_t *zone = (dns_zone_t *) event->ev_arg;
	bool free_needed, linked = false;
	dns_zone_t *raw = NULL, *secure = NULL;

	UNUSED(task);
	REQUIRE(DNS_ZONE_VALID(zone));
	INSIST(event->ev_type == DNS_EVENT_ZONECONTROL);
	INSIST(isc_refcount_current(&zone->erefs) == 0);

	zone_debuglog(zone, "zone_shutdown", 3, "shutting down");

	/*
	 * Stop things being restarted after we cancel them below.
	 */
	LOCK_ZONE(zone);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_EXITING);
	UNLOCK_ZONE(zone);

	/*
	 * If we were waiting for xfrin quota, step out of
	 * the queue.
	 * If there's no zone manager, we can't be waiting for the
	 * xfrin quota
	 */
	if (zone->zmgr != NULL) {
		RWLOCK(&zone->zmgr->rwlock, isc_rwlocktype_write);
		if (zone->statelist == &zone->zmgr->waiting_for_xfrin) {
			ISC_LIST_UNLINK(zone->zmgr->waiting_for_xfrin, zone,
					statelink);
			linked = true;
			zone->statelist = NULL;
		}
		if (zone->statelist == &zone->zmgr->xfrin_in_progress) {
			ISC_LIST_UNLINK(zone->zmgr->xfrin_in_progress, zone,
					statelink);
			zone->statelist = NULL;
			zmgr_resume_xfrs(zone->zmgr, false);
		}
		RWUNLOCK(&zone->zmgr->rwlock, isc_rwlocktype_write);
	}

	/*
	 * In task context, no locking required.  See zone_xfrdone().
	 */
	if (zone->xfr != NULL)
		dns_xfrin_shutdown(zone->xfr);

	/* Safe to release the zone now */
	if (zone->zmgr != NULL)
		dns_zonemgr_releasezone(zone->zmgr, zone);

	LOCK_ZONE(zone);
	INSIST(zone != zone->raw);
	if (linked) {
		INSIST(zone->irefs > 0);
		zone->irefs--;
	}
	if (zone->request != NULL) {
		dns_request_cancel(zone->request);
	}

	if (zone->readio != NULL)
		zonemgr_cancelio(zone->readio);

	if (zone->lctx != NULL)
		dns_loadctx_cancel(zone->lctx);

	if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_FLUSH) ||
	    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DUMPING)) {
		if (zone->writeio != NULL)
			zonemgr_cancelio(zone->writeio);

		if (zone->dctx != NULL)
			dns_dumpctx_cancel(zone->dctx);
	}

	notify_cancel(zone);

	forward_cancel(zone);

	if (zone->timer != NULL) {
		isc_timer_detach(&zone->timer);
		INSIST(zone->irefs > 0);
		zone->irefs--;
	}

	/*
	 * We have now canceled everything set the flag to allow exit_check()
	 * to succeed.	We must not unlock between setting this flag and
	 * calling exit_check().
	 */
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_SHUTDOWN);
	free_needed = exit_check(zone);
	if (inline_secure(zone)) {
		raw = zone->raw;
		zone->raw = NULL;
	}
	if (inline_raw(zone)) {
		secure = zone->secure;
		zone->secure = NULL;
	}
	UNLOCK_ZONE(zone);
	if (raw != NULL)
		dns_zone_detach(&raw);
	if (secure != NULL)
		dns_zone_idetach(&secure);
	if (free_needed)
		zone_free(zone);
}