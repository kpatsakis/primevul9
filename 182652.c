dns_zone_setmasterswithkeys(dns_zone_t *zone,
			    const isc_sockaddr_t *masters,
			    dns_name_t **keynames,
			    uint32_t count)
{
	isc_result_t result = ISC_R_SUCCESS;
	isc_sockaddr_t *newaddrs = NULL;
	isc_dscp_t *newdscps = NULL;
	dns_name_t **newnames = NULL;
	bool *newok;
	unsigned int i;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(count == 0 || masters != NULL);
	if (keynames != NULL) {
		REQUIRE(count != 0);
	}

	LOCK_ZONE(zone);
	/*
	 * The refresh code assumes that 'masters' wouldn't change under it.
	 * If it will change then kill off any current refresh in progress
	 * and update the masters info.  If it won't change then we can just
	 * unlock and exit.
	 */
	if (count != zone->masterscnt ||
	    !same_addrs(zone->masters, masters, count) ||
	    !same_keynames(zone->masterkeynames, keynames, count)) {
		if (zone->request != NULL)
			dns_request_cancel(zone->request);
	} else
		goto unlock;

	/*
	 * This needs to happen before clear_addresskeylist() sets
	 * zone->masterscnt to 0:
	 */
	if (zone->mastersok != NULL) {
		isc_mem_put(zone->mctx, zone->mastersok,
			    zone->masterscnt * sizeof(bool));
		zone->mastersok = NULL;
	}
	clear_addresskeylist(&zone->masters, &zone->masterdscps,
			     &zone->masterkeynames, &zone->masterscnt,
			     zone->mctx);
	/*
	 * If count == 0, don't allocate any space for masters, mastersok or
	 * keynames so internally, those pointers are NULL if count == 0
	 */
	if (count == 0)
		goto unlock;

	/*
	 * mastersok must contain count elements
	 */
	newok = isc_mem_get(zone->mctx, count * sizeof(*newok));
	if (newok == NULL) {
		result = ISC_R_NOMEMORY;
		isc_mem_put(zone->mctx, newaddrs, count * sizeof(*newaddrs));
		goto unlock;
	};
	for (i = 0; i < count; i++)
		newok[i] = false;

	/*
	 * Now set up the masters and masterkey lists
	 */
	result = set_addrkeylist(count, masters, &newaddrs, NULL, &newdscps,
				 keynames, &newnames, zone->mctx);
	INSIST(newdscps == NULL);
	if (result != ISC_R_SUCCESS) {
		isc_mem_put(zone->mctx, newok, count * sizeof(*newok));
		goto unlock;
	}

	/*
	 * Everything is ok so attach to the zone.
	 */
	zone->curmaster = 0;
	zone->mastersok = newok;
	zone->masters = newaddrs;
	zone->masterdscps = newdscps;
	zone->masterkeynames = newnames;
	zone->masterscnt = count;
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NOMASTERS);

 unlock:
	UNLOCK_ZONE(zone);
	return (result);
}