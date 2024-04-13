dns_zonemgr_unreachableadd(dns_zonemgr_t *zmgr, isc_sockaddr_t *remote,
			   isc_sockaddr_t *local, isc_time_t *now)
{
	uint32_t seconds = isc_time_seconds(now);
	uint32_t last = seconds;
	unsigned int i, slot = UNREACH_CHACHE_SIZE, oldest = 0;

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	RWLOCK(&zmgr->urlock, isc_rwlocktype_write);
	for (i = 0; i < UNREACH_CHACHE_SIZE; i++) {
		/* Existing entry? */
		if (isc_sockaddr_equal(&zmgr->unreachable[i].remote, remote) &&
		    isc_sockaddr_equal(&zmgr->unreachable[i].local, local))
			break;
		/* Empty slot? */
		if (zmgr->unreachable[i].expire < seconds)
			slot = i;
		/* Least recently used slot? */
		if (zmgr->unreachable[i].last < last) {
			last = zmgr->unreachable[i].last;
			oldest = i;
		}
	}
	if (i < UNREACH_CHACHE_SIZE) {
		/*
		 * Found a existing entry.  Update the expire timer and
		 * last usage timestamps.
		 */
		zmgr->unreachable[i].expire = seconds + UNREACH_HOLD_TIME;
		zmgr->unreachable[i].last = seconds;
		if (zmgr->unreachable[i].expire < seconds)
			zmgr->unreachable[i].count = 1;
		else
			zmgr->unreachable[i].count++;
	} else if (slot != UNREACH_CHACHE_SIZE) {
		/*
		 * Found a empty slot. Add a new entry to the cache.
		 */
		zmgr->unreachable[slot].expire = seconds + UNREACH_HOLD_TIME;
		zmgr->unreachable[slot].last = seconds;
		zmgr->unreachable[slot].remote = *remote;
		zmgr->unreachable[slot].local = *local;
		zmgr->unreachable[slot].count = 1;
	} else {
		/*
		 * Replace the least recently used entry in the cache.
		 */
		zmgr->unreachable[oldest].expire = seconds + UNREACH_HOLD_TIME;
		zmgr->unreachable[oldest].last = seconds;
		zmgr->unreachable[oldest].remote = *remote;
		zmgr->unreachable[oldest].local = *local;
		zmgr->unreachable[oldest].count = 1;
	}
	RWUNLOCK(&zmgr->urlock, isc_rwlocktype_write);
}