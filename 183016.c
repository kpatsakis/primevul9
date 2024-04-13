zone_rdclass_tostr(dns_zone_t *zone, char *buf, size_t length) {
	isc_buffer_t buffer;

	REQUIRE(buf != NULL);
	REQUIRE(length > 1U);

	/*
	 * Leave space for terminating '\0'.
	 */
	isc_buffer_init(&buffer, buf, (unsigned int)length - 1);
	(void)dns_rdataclass_totext(zone->rdclass, &buffer);

	buf[isc_buffer_usedlength(&buffer)] = '\0';
}