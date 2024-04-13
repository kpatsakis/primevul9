dns_zone_setfile(dns_zone_t *zone, const char *file,
		 dns_masterformat_t format,
		 const dns_master_style_t *style)
{
	isc_result_t result = ISC_R_SUCCESS;

	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	result = dns_zone_setstring(zone, &zone->masterfile, file);
	if (result == ISC_R_SUCCESS) {
		zone->masterformat = format;
		if (format == dns_masterformat_text)
			zone->masterstyle = style;
		result = default_journal(zone);
	}
	UNLOCK_ZONE(zone);

	return (result);
}