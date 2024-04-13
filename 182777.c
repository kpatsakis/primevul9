notify_find_address(dns_notify_t *notify) {
	isc_result_t result;
	unsigned int options;

	REQUIRE(DNS_NOTIFY_VALID(notify));
	options = DNS_ADBFIND_WANTEVENT | DNS_ADBFIND_INET |
		  DNS_ADBFIND_INET6 | DNS_ADBFIND_RETURNLAME;

	if (notify->zone->view->adb == NULL)
		goto destroy;

	result = dns_adb_createfind(notify->zone->view->adb,
				    notify->zone->task,
				    process_adb_event, notify,
				    &notify->ns, dns_rootname, 0,
				    options, 0, NULL,
				    notify->zone->view->dstport,
				    0, NULL, &notify->find);

	/* Something failed? */
	if (result != ISC_R_SUCCESS)
		goto destroy;

	/* More addresses pending? */
	if ((notify->find->options & DNS_ADBFIND_WANTEVENT) != 0)
		return;

	/* We have as many addresses as we can get. */
	LOCK_ZONE(notify->zone);
	notify_send(notify);
	UNLOCK_ZONE(notify->zone);

 destroy:
	notify_destroy(notify, false);
}