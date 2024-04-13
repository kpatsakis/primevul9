zone_registerinclude(const char *filename, void *arg) {
	isc_result_t result;
	dns_zone_t *zone = (dns_zone_t *) arg;
	dns_include_t *inc = NULL;

	REQUIRE(DNS_ZONE_VALID(zone));

	if (filename == NULL)
		return;

	/*
	 * Suppress duplicates.
	 */
	for (inc = ISC_LIST_HEAD(zone->newincludes);
	     inc != NULL;
	     inc = ISC_LIST_NEXT(inc, link))
		if (strcmp(filename, inc->name) == 0)
			return;

	inc = isc_mem_get(zone->mctx, sizeof(dns_include_t));
	if (inc == NULL)
		return;
	inc->name = isc_mem_strdup(zone->mctx, filename);
	if (inc->name == NULL) {
		isc_mem_put(zone->mctx, inc, sizeof(dns_include_t));
		return;
	}
	ISC_LINK_INIT(inc, link);

	result = isc_file_getmodtime(filename, &inc->filetime);
	if (result != ISC_R_SUCCESS)
		isc_time_settoepoch(&inc->filetime);

	ISC_LIST_APPEND(zone->newincludes, inc, link);
}