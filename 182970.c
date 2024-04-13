zone_startload(dns_db_t *db, dns_zone_t *zone, isc_time_t loadtime) {
	dns_load_t *load;
	isc_result_t result;
	isc_result_t tresult;
	unsigned int options;

	dns_zone_rpz_enable_db(zone, db);
	dns_zone_catz_enable_db(zone, db);
	options = get_master_options(zone);
	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_MANYERRORS))
		options |= DNS_MASTER_MANYERRORS;

	if (zone->zmgr != NULL && zone->db != NULL && zone->loadtask != NULL) {
		load = isc_mem_get(zone->mctx, sizeof(*load));
		if (load == NULL)
			return (ISC_R_NOMEMORY);

		load->mctx = NULL;
		load->zone = NULL;
		load->db = NULL;
		load->loadtime = loadtime;
		load->magic = LOAD_MAGIC;

		isc_mem_attach(zone->mctx, &load->mctx);
		zone_iattach(zone, &load->zone);
		dns_db_attach(db, &load->db);
		dns_rdatacallbacks_init(&load->callbacks);
		load->callbacks.rawdata = zone_setrawdata;
		zone_iattach(zone, &load->callbacks.zone);
		result = dns_db_beginload(db, &load->callbacks);
		if (result != ISC_R_SUCCESS)
			goto cleanup;
		result = zonemgr_getio(zone->zmgr, true, zone->loadtask,
				       zone_gotreadhandle, load,
				       &zone->readio);
		if (result != ISC_R_SUCCESS) {
			/*
			 * We can't report multiple errors so ignore
			 * the result of dns_db_endload().
			 */
			(void)dns_db_endload(load->db, &load->callbacks);
			goto cleanup;
		} else
			result = DNS_R_CONTINUE;
	} else {
		dns_rdatacallbacks_t callbacks;

		dns_rdatacallbacks_init(&callbacks);
		callbacks.rawdata = zone_setrawdata;
		zone_iattach(zone, &callbacks.zone);
		result = dns_db_beginload(db, &callbacks);
		if (result != ISC_R_SUCCESS) {
			zone_idetach(&callbacks.zone);
			return (result);
		}
		result = dns_master_loadfile(zone->masterfile,
					     &zone->origin, &zone->origin,
					     zone->rdclass, options, 0,
					     &callbacks,
					     zone_registerinclude,
					     zone, zone->mctx,
					     zone->masterformat,
					     zone->maxttl);
		tresult = dns_db_endload(db, &callbacks);
		if (result == ISC_R_SUCCESS)
			result = tresult;
		zone_idetach(&callbacks.zone);
	}

	return (result);

 cleanup:
	load->magic = 0;
	dns_db_detach(&load->db);
	zone_idetach(&load->zone);
	zone_idetach(&load->callbacks.zone);
	isc_mem_detach(&load->mctx);
	isc_mem_put(zone->mctx, load, sizeof(*load));
	return (result);
}