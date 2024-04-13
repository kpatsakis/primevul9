dns_zonemgr_forcemaint(dns_zonemgr_t *zmgr) {
	dns_zone_t *p;

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	RWLOCK(&zmgr->rwlock, isc_rwlocktype_read);
	for (p = ISC_LIST_HEAD(zmgr->zones);
	     p != NULL;
	     p = ISC_LIST_NEXT(p, link))
	{
		dns_zone_maintenance(p);
	}
	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_read);

	/*
	 * Recent configuration changes may have increased the
	 * amount of available transfers quota.  Make sure any
	 * transfers currently blocked on quota get started if
	 * possible.
	 */
	RWLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	zmgr_resume_xfrs(zmgr, true);
	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	return (ISC_R_SUCCESS);
}