dns_zone_getdbtype(dns_zone_t *zone, char ***argv, isc_mem_t *mctx) {
	size_t size = 0;
	unsigned int i;
	isc_result_t result = ISC_R_SUCCESS;
	void *mem;
	char **tmp, *tmp2, *base;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(argv != NULL && *argv == NULL);

	LOCK_ZONE(zone);
	size = (zone->db_argc + 1) * sizeof(char *);
	for (i = 0; i < zone->db_argc; i++)
		size += strlen(zone->db_argv[i]) + 1;
	mem = isc_mem_allocate(mctx, size);
	if (mem != NULL) {
		tmp = mem;
		tmp2 = mem;
		base = mem;
		tmp2 += (zone->db_argc + 1) * sizeof(char *);
		for (i = 0; i < zone->db_argc; i++) {
			*tmp++ = tmp2;
			strlcpy(tmp2, zone->db_argv[i], size - (tmp2 - base));
			tmp2 += strlen(tmp2) + 1;
		}
		*tmp = NULL;
	} else
		result = ISC_R_NOMEMORY;
	UNLOCK_ZONE(zone);
	*argv = mem;
	return (result);
}