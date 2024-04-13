dns_zone_setdbtype(dns_zone_t *zone,
		   unsigned int dbargc, const char * const *dbargv)
{
	isc_result_t result = ISC_R_SUCCESS;
	char **argv = NULL;
	unsigned int i;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(dbargc >= 1);
	REQUIRE(dbargv != NULL);

	LOCK_ZONE(zone);

	/* Set up a new database argument list. */
	argv = isc_mem_get(zone->mctx, dbargc * sizeof(*argv));
	if (argv == NULL) {
		goto nomem;
	}
	for (i = 0; i < dbargc; i++) {
		argv[i] = NULL;
	}
	for (i = 0; i < dbargc; i++) {
		argv[i] = isc_mem_strdup(zone->mctx, dbargv[i]);
		if (argv[i] == NULL)
			goto nomem;
	}

	/* Free the old list. */
	zone_freedbargs(zone);

	zone->db_argc = dbargc;
	zone->db_argv = argv;
	result = ISC_R_SUCCESS;
	goto unlock;

 nomem:
	if (argv != NULL) {
		for (i = 0; i < dbargc; i++) {
			if (argv[i] != NULL) {
				isc_mem_free(zone->mctx, argv[i]);
			}
		}
		isc_mem_put(zone->mctx, argv, dbargc * sizeof(*argv));
	}
	result = ISC_R_NOMEMORY;

 unlock:
	UNLOCK_ZONE(zone);
	return (result);
}