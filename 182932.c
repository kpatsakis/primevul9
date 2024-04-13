default_journal(dns_zone_t *zone) {
	isc_result_t result;
	char *journal;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(LOCKED_ZONE(zone));

	if (zone->masterfile != NULL) {
		/* Calculate string length including '\0'. */
		int len = strlen(zone->masterfile) + sizeof(".jnl");
		journal = isc_mem_allocate(zone->mctx, len);
		if (journal == NULL)
			return (ISC_R_NOMEMORY);
		strlcpy(journal, zone->masterfile, len);
		strlcat(journal, ".jnl", len);
	} else {
		journal = NULL;
	}
	result = dns_zone_setstring(zone, &zone->journal, journal);
	if (journal != NULL)
		isc_mem_free(zone->mctx, journal);
	return (result);
}