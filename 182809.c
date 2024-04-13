dns_zone_setstring(dns_zone_t *zone, char **field, const char *value) {
	char *copy;

	if (value != NULL) {
		copy = isc_mem_strdup(zone->mctx, value);
		if (copy == NULL)
			return (ISC_R_NOMEMORY);
	} else {
		copy = NULL;
	}

	if (*field != NULL)
		isc_mem_free(zone->mctx, *field);

	*field = copy;
	return (ISC_R_SUCCESS);
}