zone_touched(dns_zone_t *zone) {
	isc_result_t result;
	isc_time_t modtime;
	dns_include_t *include;

	REQUIRE(DNS_ZONE_VALID(zone));

	result = isc_file_getmodtime(zone->masterfile, &modtime);
	if (result != ISC_R_SUCCESS ||
	    isc_time_compare(&modtime, &zone->loadtime) > 0)
		return (true);

	for (include = ISC_LIST_HEAD(zone->includes);
	     include != NULL;
	     include = ISC_LIST_NEXT(include, link))
	{
		result = isc_file_getmodtime(include->name, &modtime);
		if (result != ISC_R_SUCCESS ||
		    isc_time_compare(&modtime, &include->filetime) > 0)
			return (true);
	}

	return (false);
}