dns_zone_loadandthaw(dns_zone_t *zone) {
	isc_result_t result;

	if (inline_raw(zone))
		result = zone_load(zone->secure, DNS_ZONELOADFLAG_THAW,
				   false);
	else
		result = zone_load(zone, DNS_ZONELOADFLAG_THAW, false);

	switch (result) {
	case DNS_R_CONTINUE:
		/* Deferred thaw. */
		break;
	case DNS_R_UPTODATE:
	case ISC_R_SUCCESS:
	case DNS_R_SEENINCLUDE:
		zone->update_disabled = false;
		break;
	case DNS_R_NOMASTERFILE:
		zone->update_disabled = false;
		break;
	default:
		/* Error, remain in disabled state. */
		break;
	}
	return (result);
}