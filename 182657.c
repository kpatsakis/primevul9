zone_name_tostr(dns_zone_t *zone, char *buf, size_t length) {
	isc_result_t result = ISC_R_FAILURE;
	isc_buffer_t buffer;

	REQUIRE(buf != NULL);
	REQUIRE(length > 1U);

	/*
	 * Leave space for terminating '\0'.
	 */
	isc_buffer_init(&buffer, buf, (unsigned int)length - 1);
	if (dns_name_dynamic(&zone->origin))
		result = dns_name_totext(&zone->origin, true, &buffer);
	if (result != ISC_R_SUCCESS &&
	    isc_buffer_availablelength(&buffer) >= (sizeof("<UNKNOWN>") - 1))
		isc_buffer_putstr(&buffer, "<UNKNOWN>");

	buf[isc_buffer_usedlength(&buffer)] = '\0';
}