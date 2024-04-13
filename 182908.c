dns_zone_setalsonotifydscpkeys(dns_zone_t *zone, const isc_sockaddr_t *notify,
			       const isc_dscp_t *dscps, dns_name_t **keynames,
			       uint32_t count)
{
	isc_result_t result;
	isc_sockaddr_t *newaddrs = NULL;
	isc_dscp_t *newdscps = NULL;
	dns_name_t **newnames = NULL;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(count == 0 || notify != NULL);
	if (keynames != NULL)
		REQUIRE(count != 0);

	LOCK_ZONE(zone);

	if (count == zone->notifycnt &&
	    same_addrs(zone->notify, notify, count) &&
	    same_keynames(zone->notifykeynames, keynames, count))
		goto unlock;

	clear_addresskeylist(&zone->notify, &zone->notifydscp,
			     &zone->notifykeynames, &zone->notifycnt,
			     zone->mctx);

	if (count == 0)
		goto unlock;

	/*
	 * Set up the notify and notifykey lists
	 */
	result = set_addrkeylist(count, notify, &newaddrs, dscps, &newdscps,
				 keynames, &newnames, zone->mctx);
	if (result != ISC_R_SUCCESS)
		goto unlock;

	/*
	 * Everything is ok so attach to the zone.
	 */
	zone->notify = newaddrs;
	zone->notifydscp = newdscps;
	zone->notifykeynames = newnames;
	zone->notifycnt = count;
 unlock:
	UNLOCK_ZONE(zone);
	return (ISC_R_SUCCESS);
}