zonemgr_free(dns_zonemgr_t *zmgr) {
	isc_mem_t *mctx;

	INSIST(zmgr->refs == 0);
	INSIST(ISC_LIST_EMPTY(zmgr->zones));

	zmgr->magic = 0;

	isc_mutex_destroy(&zmgr->iolock);
	isc_ratelimiter_detach(&zmgr->notifyrl);
	isc_ratelimiter_detach(&zmgr->refreshrl);
	isc_ratelimiter_detach(&zmgr->startupnotifyrl);
	isc_ratelimiter_detach(&zmgr->startuprefreshrl);

	isc_rwlock_destroy(&zmgr->urlock);
	isc_rwlock_destroy(&zmgr->rwlock);
	mctx = zmgr->mctx;
	isc_mem_put(zmgr->mctx, zmgr, sizeof(*zmgr));
	isc_mem_detach(&mctx);
}