zone_saveunique(dns_zone_t *zone, const char *path, const char *templat) {
	char *buf;
	int buflen;
	isc_result_t result;

	buflen = strlen(path) + strlen(templat) + 2;

	buf = isc_mem_get(zone->mctx, buflen);
	if (buf == NULL)
		return;

	result = isc_file_template(path, templat, buf, buflen);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	result = isc_file_renameunique(path, buf);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	dns_zone_log(zone, ISC_LOG_WARNING, "unable to load from '%s'; "
		     "renaming file to '%s' for failure analysis and "
		     "retransferring.", path, buf);

 cleanup:
	isc_mem_put(zone->mctx, buf, buflen);
}