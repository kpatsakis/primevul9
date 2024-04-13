zone_viewname_tostr(dns_zone_t *zone, char *buf, size_t length) {
	isc_buffer_t buffer;

	REQUIRE(buf != NULL);
	REQUIRE(length > 1U);


	/*
	 * Leave space for terminating '\0'.
	 */
	isc_buffer_init(&buffer, buf, (unsigned int)length - 1);

	if (zone->view == NULL) {
		isc_buffer_putstr(&buffer, "_none");
	} else if (strlen(zone->view->name)
		   < isc_buffer_availablelength(&buffer)) {
		isc_buffer_putstr(&buffer, zone->view->name);
	} else {
		isc_buffer_putstr(&buffer, "_toolong");
	}

	buf[isc_buffer_usedlength(&buffer)] = '\0';
}