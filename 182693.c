zone_gotwritehandle(isc_task_t *task, isc_event_t *event) {
	const char me[] = "zone_gotwritehandle";
	dns_zone_t *zone = event->ev_arg;
	isc_result_t result = ISC_R_SUCCESS;
	dns_dbversion_t *version = NULL;
	dns_masterrawheader_t rawdata;

	REQUIRE(DNS_ZONE_VALID(zone));
	INSIST(task == zone->task);
	ENTER;

	if ((event->ev_attributes & ISC_EVENTATTR_CANCELED) != 0)
		result = ISC_R_CANCELED;
	isc_event_free(&event);
	if (result == ISC_R_CANCELED)
		goto fail;

	LOCK_ZONE(zone);
	INSIST(zone != zone->raw);
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL) {
		const dns_master_style_t *output_style;

		dns_db_currentversion(zone->db, &version);
		dns_master_initrawheader(&rawdata);
		if (inline_secure(zone))
			get_raw_serial(zone->raw, &rawdata);
		if (zone->type == dns_zone_key)
			output_style = &dns_master_style_keyzone;
		else if (zone->masterstyle != NULL)
			output_style = zone->masterstyle;
		else
			output_style = &dns_master_style_default;
		result = dns_master_dumpinc(zone->mctx, zone->db, version,
					    output_style, zone->masterfile,
					    zone->task, dump_done, zone,
					    &zone->dctx, zone->masterformat,
					    &rawdata);
		dns_db_closeversion(zone->db, &version, false);
	} else
		result = ISC_R_CANCELED;
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	UNLOCK_ZONE(zone);
	if (result != DNS_R_CONTINUE)
		goto fail;
	return;

 fail:
	dump_done(zone, result);
}