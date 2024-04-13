notify_destroy(dns_notify_t *notify, bool locked) {
	isc_mem_t *mctx;

	REQUIRE(DNS_NOTIFY_VALID(notify));

	if (notify->zone != NULL) {
		if (!locked)
			LOCK_ZONE(notify->zone);
		REQUIRE(LOCKED_ZONE(notify->zone));
		if (ISC_LINK_LINKED(notify, link))
			ISC_LIST_UNLINK(notify->zone->notifies, notify, link);
		if (!locked)
			UNLOCK_ZONE(notify->zone);
		if (locked)
			zone_idetach(&notify->zone);
		else
			dns_zone_idetach(&notify->zone);
	}
	if (notify->find != NULL)
		dns_adb_destroyfind(&notify->find);
	if (notify->request != NULL)
		dns_request_destroy(&notify->request);
	if (dns_name_dynamic(&notify->ns))
		dns_name_free(&notify->ns, notify->mctx);
	if (notify->key != NULL)
		dns_tsigkey_detach(&notify->key);
	mctx = notify->mctx;
	isc_mem_put(notify->mctx, notify, sizeof(*notify));
	isc_mem_detach(&mctx);
}