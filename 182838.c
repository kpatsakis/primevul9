dns_zonemgr_detach(dns_zonemgr_t **zmgrp) {
	dns_zonemgr_t *zmgr;
	bool free_now = false;

	REQUIRE(zmgrp != NULL);
	zmgr = *zmgrp;
	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	RWLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	zmgr->refs--;
	if (zmgr->refs == 0)
		free_now = true;
	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_write);

	if (free_now)
		zonemgr_free(zmgr);
	*zmgrp = NULL;
}