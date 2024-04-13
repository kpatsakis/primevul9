dns_zonemgr_getcount(dns_zonemgr_t *zmgr, int state) {
	dns_zone_t *zone;
	unsigned int count = 0;

	REQUIRE(DNS_ZONEMGR_VALID(zmgr));

	RWLOCK(&zmgr->rwlock, isc_rwlocktype_read);
	switch (state) {
	case DNS_ZONESTATE_XFERRUNNING:
		for (zone = ISC_LIST_HEAD(zmgr->xfrin_in_progress);
		     zone != NULL;
		     zone = ISC_LIST_NEXT(zone, statelink))
			count++;
		break;
	case DNS_ZONESTATE_XFERDEFERRED:
		for (zone = ISC_LIST_HEAD(zmgr->waiting_for_xfrin);
		     zone != NULL;
		     zone = ISC_LIST_NEXT(zone, statelink))
			count++;
		break;
	case DNS_ZONESTATE_SOAQUERY:
		for (zone = ISC_LIST_HEAD(zmgr->zones);
		     zone != NULL;
		     zone = ISC_LIST_NEXT(zone, link))
			if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_REFRESH))
				count++;
		break;
	case DNS_ZONESTATE_ANY:
		for (zone = ISC_LIST_HEAD(zmgr->zones);
		     zone != NULL;
		     zone = ISC_LIST_NEXT(zone, link)) {
			dns_view_t *view = zone->view;
			if (view != NULL && strcmp(view->name, "_bind") == 0)
				continue;
			count++;
		}
		break;
	case DNS_ZONESTATE_AUTOMATIC:
		for (zone = ISC_LIST_HEAD(zmgr->zones);
		     zone != NULL;
		     zone = ISC_LIST_NEXT(zone, link)) {
			dns_view_t *view = zone->view;
			if (view != NULL && strcmp(view->name, "_bind") == 0)
				continue;
			if (zone->automatic)
				count++;
		}
		break;
	default:
		INSIST(0);
		ISC_UNREACHABLE();
	}

	RWUNLOCK(&zmgr->rwlock, isc_rwlocktype_read);

	return (count);
}