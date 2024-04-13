zone_namerd_tostr(dns_zone_t *zone, char *buf, size_t length) {
	isc_result_t result = ISC_R_FAILURE;
	isc_buffer_t buffer;

	REQUIRE(buf != NULL);
	REQUIRE(length > 1U);

	/*
	 * Leave space for terminating '\0'.
	 */
	isc_buffer_init(&buffer, buf, (unsigned int)length - 1);
	if (zone->type != dns_zone_redirect && zone->type != dns_zone_key) {
		if (dns_name_dynamic(&zone->origin))
			result = dns_name_totext(&zone->origin, true, &buffer);
		if (result != ISC_R_SUCCESS &&
		    isc_buffer_availablelength(&buffer) >= (sizeof("<UNKNOWN>") - 1))
			isc_buffer_putstr(&buffer, "<UNKNOWN>");

		if (isc_buffer_availablelength(&buffer) > 0)
			isc_buffer_putstr(&buffer, "/");
		(void)dns_rdataclass_totext(zone->rdclass, &buffer);
	}

	if (zone->view != NULL && strcmp(zone->view->name, "_bind") != 0 &&
	    strcmp(zone->view->name, "_default") != 0 &&
	    strlen(zone->view->name) < isc_buffer_availablelength(&buffer)) {
		isc_buffer_putstr(&buffer, "/");
		isc_buffer_putstr(&buffer, zone->view->name);
	}
	if (inline_secure(zone) && 9U < isc_buffer_availablelength(&buffer))
		isc_buffer_putstr(&buffer, " (signed)");
	if (inline_raw(zone) && 11U < isc_buffer_availablelength(&buffer))
		isc_buffer_putstr(&buffer, " (unsigned)");

	buf[isc_buffer_usedlength(&buffer)] = '\0';
}