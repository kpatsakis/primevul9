maybe_send_secure(dns_zone_t *zone) {
	isc_result_t result;

	/*
	 * We've finished loading, or else failed to load, an inline-signing
	 * 'secure' zone.  We now need information about the status of the
	 * 'raw' zone.  If we failed to load, then we need it to send a
	 * copy of its database; if we succeeded, we need it to send its
	 * serial number so that we can sync with it.  If it has not yet
	 * loaded, we set a flag so that it will send the necessary
	 * information when it has finished loading.
	 */
	if (zone->raw->db != NULL) {
		if (zone->db != NULL) {
			uint32_t serial;
			unsigned int soacount;

			result = zone_get_from_db(zone->raw, zone->raw->db,
						  NULL, &soacount, &serial,
						  NULL, NULL, NULL, NULL, NULL);
			if (result == ISC_R_SUCCESS && soacount > 0U)
				zone_send_secureserial(zone->raw, serial);
		} else
			zone_send_securedb(zone->raw, zone->raw->db);

	} else
		DNS_ZONE_SETFLAG(zone->raw, DNS_ZONEFLG_SENDSECURE);
}