dns_zone_getincludes(dns_zone_t *zone, char ***includesp) {
	dns_include_t *include;
	char **array = NULL;
	unsigned int n = 0;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(includesp != NULL && *includesp == NULL);

	LOCK_ZONE(zone);
	if (zone->nincludes == 0)
		goto done;

	array = isc_mem_allocate(zone->mctx, sizeof(char *) * zone->nincludes);
	if (array == NULL)
		goto done;
	for (include = ISC_LIST_HEAD(zone->includes);
	     include != NULL;
	     include = ISC_LIST_NEXT(include, link)) {
		INSIST(n < zone->nincludes);
		array[n++] = isc_mem_strdup(zone->mctx, include->name);
	}
	INSIST(n == zone->nincludes);
	*includesp = array;

 done:
	UNLOCK_ZONE(zone);
	return (n);
}