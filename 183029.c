dns_zone_refresh(dns_zone_t *zone) {
	isc_interval_t i;
	uint32_t oldflags;
	unsigned int j;
	isc_result_t result;

	REQUIRE(DNS_ZONE_VALID(zone));

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING))
		return;

	/*
	 * Set DNS_ZONEFLG_REFRESH so that there is only one refresh operation
	 * in progress at a time.
	 */

	LOCK_ZONE(zone);
	oldflags = zone->flags;
	if (zone->masterscnt == 0) {
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOMASTERS);
		if ((oldflags & DNS_ZONEFLG_NOMASTERS) == 0)
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "cannot refresh: no masters");
		goto unlock;
	}
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_REFRESH);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NOEDNS);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_USEALTXFRSRC);
	if ((oldflags & (DNS_ZONEFLG_REFRESH|DNS_ZONEFLG_LOADING)) != 0)
		goto unlock;

	/*
	 * Set the next refresh time as if refresh check has failed.
	 * Setting this to the retry time will do that.  XXXMLG
	 * If we are successful it will be reset using zone->refresh.
	 */
	isc_interval_set(&i, zone->retry - isc_random_uniform(zone->retry / 4),
			 0);
	result = isc_time_nowplusinterval(&zone->refreshtime, &i);
	if (result != ISC_R_SUCCESS)
		dns_zone_log(zone, ISC_LOG_WARNING,
			     "isc_time_nowplusinterval() failed: %s",
			     dns_result_totext(result));

	/*
	 * When lacking user-specified timer values from the SOA,
	 * do exponential backoff of the retry time up to a
	 * maximum of six hours.
	 */
	if (! DNS_ZONE_FLAG(zone, DNS_ZONEFLG_HAVETIMERS))
		zone->retry = ISC_MIN(zone->retry * 2, 6 * 3600);

	zone->curmaster = 0;
	for (j = 0; j < zone->masterscnt; j++)
		zone->mastersok[j] = false;
	/* initiate soa query */
	queue_soa_query(zone);
 unlock:
	UNLOCK_ZONE(zone);
}