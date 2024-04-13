setmodtime(dns_zone_t *zone, isc_time_t *expiretime) {
	isc_result_t result;
	isc_time_t when;
	isc_interval_t i;

	isc_interval_set(&i, zone->expire, 0);
	result = isc_time_subtract(expiretime, &i, &when);
	if (result != ISC_R_SUCCESS)
		return;

	result = ISC_R_FAILURE;
	if (zone->journal != NULL)
		result = isc_file_settime(zone->journal, &when);
	if (result == ISC_R_SUCCESS &&
	    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDDUMP) &&
	    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDDUMP))
		result = isc_file_settime(zone->masterfile, &when);
	else if (result != ISC_R_SUCCESS)
		result = isc_file_settime(zone->masterfile, &when);

	/*
	 * Someone removed the file from underneath us!
	 */
	if (result == ISC_R_FILENOTFOUND) {
		zone_needdump(zone, DNS_DUMP_DELAY);
	} else if (result != ISC_R_SUCCESS)
		dns_zone_log(zone, ISC_LOG_ERROR, "refresh: could not set "
			     "file modification time of '%s': %s",
			     zone->masterfile, dns_result_totext(result));
}