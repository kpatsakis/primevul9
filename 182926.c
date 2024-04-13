queue_xfrin(dns_zone_t *zone) {
	const char me[] = "queue_xfrin";
	isc_result_t result;
	dns_zonemgr_t *zmgr = zone->zmgr;

	ENTER;

	INSIST(zone->statelist == NULL);

	RWLOCK(&zmgr->rwlock, isc_rwlocktype_write);
	ISC_LIST_APPEND(zmgr->waiting_for_xfrin, zone, statelink);
	LOCK_ZONE(zone);
	zone->irefs++;
	UNLOCK_ZONE(zone);
	zone->statelist = &zmgr->waiting_for_xfrin;
	result = zmgr_start_xfrin_ifquota(zmgr, zone);
	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_write);

	if (result == ISC_R_QUOTA) {
		dns_zone_logc(zone, DNS_LOGCATEGORY_XFER_IN, ISC_LOG_INFO,
			      "zone transfer deferred due to quota");
	} else if (result != ISC_R_SUCCESS) {
		dns_zone_logc(zone, DNS_LOGCATEGORY_XFER_IN, ISC_LOG_ERROR,
			      "starting zone transfer: %s",
			      isc_result_totext(result));
	}
}