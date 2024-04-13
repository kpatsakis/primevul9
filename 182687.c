dns_zonemgr_unreachable(dns_zonemgr_t *zmgr, isc_sockaddr_t *remote,
			isc_sockaddr_t *local, isc_time_t *now)
{
	unsigned int i;
	isc_rwlocktype_t locktype;
	isc_result_t result;
	uint32_t seconds = isc_time_seconds(now);
	uint32_t count = 0;

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	locktype = isc_rwlocktype_read;
	RWLOCK(&zmgr->urlock, locktype);
	for (i = 0; i < UNREACH_CHACHE_SIZE; i++) {
		if (zmgr->unreachable[i].expire >= seconds &&
		    isc_sockaddr_equal(&zmgr->unreachable[i].remote, remote) &&
		    isc_sockaddr_equal(&zmgr->unreachable[i].local, local)) {
			result = isc_rwlock_tryupgrade(&zmgr->urlock);
			if (result == ISC_R_SUCCESS) {
				locktype = isc_rwlocktype_write;
				zmgr->unreachable[i].last = seconds;
				count = zmgr->unreachable[i].count;
			}
			break;
		}
	}
	RWUNLOCK(&zmgr->urlock, locktype);
	return (i < UNREACH_CHACHE_SIZE && count > 1U);
}