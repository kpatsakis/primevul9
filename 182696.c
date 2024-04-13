forward_cancel(dns_zone_t *zone) {
	dns_forward_t *forward;

	/*
	 * 'zone' locked by caller.
	 */

	REQUIRE(LOCKED_ZONE(zone));

	for (forward = ISC_LIST_HEAD(zone->forwards);
	     forward != NULL;
	     forward = ISC_LIST_NEXT(forward, link)) {
		if (forward->request != NULL)
			dns_request_cancel(forward->request);
	}
}