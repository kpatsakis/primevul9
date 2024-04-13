dns_zonemgr_shutdown(dns_zonemgr_t *zmgr) {
	dns_zone_t *zone;

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	isc_ratelimiter_shutdown(zmgr->notifyrl);
	isc_ratelimiter_shutdown(zmgr->refreshrl);
	isc_ratelimiter_shutdown(zmgr->startupnotifyrl);
	isc_ratelimiter_shutdown(zmgr->startuprefreshrl);

	if (zmgr->task != NULL)
		isc_task_destroy(&zmgr->task);
	if (zmgr->zonetasks != NULL)
		isc_taskpool_destroy(&zmgr->zonetasks);
	if (zmgr->loadtasks != NULL)
		isc_taskpool_destroy(&zmgr->loadtasks);
	if (zmgr->mctxpool != NULL)
		isc_pool_destroy(&zmgr->mctxpool);

	RWLOCK(&zmgr->rwlock, isc_rwlocktype_read);
	for (zone = ISC_LIST_HEAD(zmgr->zones);
	     zone != NULL;
	     zone = ISC_LIST_NEXT(zone, link))
	{
		LOCK_ZONE(zone);
		forward_cancel(zone);
		UNLOCK_ZONE(zone);
	}
	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_read);
}