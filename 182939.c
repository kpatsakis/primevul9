zone_dump(dns_zone_t *zone, bool compact) {
	const char me[] = "zone_dump";
	isc_result_t result;
	dns_dbversion_t *version = NULL;
	bool again;
	dns_db_t *db = NULL;
	char *masterfile = NULL;
	dns_masterformat_t masterformat = dns_masterformat_none;

/*
 * 'compact' MUST only be set if we are task locked.
 */

	REQUIRE(DNS_ZONE_VALID(zone));
	ENTER;

 redo:
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL)
		dns_db_attach(zone->db, &db);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	LOCK_ZONE(zone);
	if (zone->masterfile != NULL) {
		masterfile = isc_mem_strdup(zone->mctx, zone->masterfile);
		masterformat = zone->masterformat;
	}
	UNLOCK_ZONE(zone);
	if (db == NULL) {
		result = DNS_R_NOTLOADED;
		goto fail;
	}
	if (masterfile == NULL) {
		result = DNS_R_NOMASTERFILE;
		goto fail;
	}

	if (compact && zone->type != dns_zone_stub) {
		dns_zone_t *dummy = NULL;
		LOCK_ZONE(zone);
		zone_iattach(zone, &dummy);
		result = zonemgr_getio(zone->zmgr, false, zone->task,
				       zone_gotwritehandle, zone,
				       &zone->writeio);
		if (result != ISC_R_SUCCESS)
			zone_idetach(&dummy);
		else
			result = DNS_R_CONTINUE;
		UNLOCK_ZONE(zone);
	} else {
		const dns_master_style_t *output_style;

		dns_masterrawheader_t rawdata;
		dns_db_currentversion(db, &version);
		dns_master_initrawheader(&rawdata);
		if (inline_secure(zone))
			get_raw_serial(zone->raw, &rawdata);
		if (zone->type == dns_zone_key)
			output_style = &dns_master_style_keyzone;
		else
			output_style = &dns_master_style_default;
		result = dns_master_dump(zone->mctx, db, version,
					 output_style, masterfile,
					 masterformat, &rawdata);
		dns_db_closeversion(db, &version, false);
	}
 fail:
	if (db != NULL)
		dns_db_detach(&db);
	if (masterfile != NULL)
		isc_mem_free(zone->mctx, masterfile);
	masterfile = NULL;

	if (result == DNS_R_CONTINUE)
		return (ISC_R_SUCCESS); /* XXXMPA */

	again = false;
	LOCK_ZONE(zone);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_DUMPING);
	if (result != ISC_R_SUCCESS) {
		/*
		 * Try again in a short while.
		 */
		zone_needdump(zone, DNS_DUMP_DELAY);
	} else if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_FLUSH) &&
		   DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDDUMP) &&
		   DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED)) {
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NEEDDUMP);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_DUMPING);
		isc_time_settoepoch(&zone->dumptime);
		again = true;
	} else
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_FLUSH);
	UNLOCK_ZONE(zone);
	if (again)
		goto redo;

	return (result);
}