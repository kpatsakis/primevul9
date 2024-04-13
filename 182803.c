dns_zonemgr_unreachabledel(dns_zonemgr_t *zmgr, isc_sockaddr_t *remote,
			   isc_sockaddr_t *local)
{
	unsigned int i;
	isc_rwlocktype_t locktype;
	isc_result_t result;

	char master[ISC_SOCKADDR_FORMATSIZE];
	char source[ISC_SOCKADDR_FORMATSIZE];

	isc_sockaddr_format(remote, master, sizeof(master));
	isc_sockaddr_format(local, source, sizeof(source));

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	locktype = isc_rwlocktype_read;
	RWLOCK(&zmgr->urlock, locktype);
	for (i = 0; i < UNREACH_CHACHE_SIZE; i++) {
		if (isc_sockaddr_equal(&zmgr->unreachable[i].remote, remote) &&
		    isc_sockaddr_equal(&zmgr->unreachable[i].local, local)) {
			if (zmgr->unreachable[i].expire == 0)
				break;
			result = isc_rwlock_tryupgrade(&zmgr->urlock);
			if (result == ISC_R_SUCCESS) {
				locktype = isc_rwlocktype_write;
				zmgr->unreachable[i].expire = 0;
				isc_log_write(dns_lctx, DNS_LOGCATEGORY_GENERAL,
					      DNS_LOGMODULE_ZONE, ISC_LOG_INFO,
					      "master %s (source %s) deleted "
					      "from unreachable cache",
					      master, source);
			}
			break;
		}
	}
	RWUNLOCK(&zmgr->urlock, locktype);
}