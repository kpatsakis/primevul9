notify_cancel(dns_zone_t *zone) {
	dns_notify_t *notify;

	/*
	 * 'zone' locked by caller.
	 */

	REQUIRE(LOCKED_ZONE(zone));

	for (notify = ISC_LIST_HEAD(zone->notifies);
	     notify != NULL;
	     notify = ISC_LIST_NEXT(notify, link)) {
		if (notify->find != NULL)
			dns_adb_cancelfind(notify->find);
		if (notify->request != NULL)
			dns_request_cancel(notify->request);
	}
}