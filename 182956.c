resume_signingwithkey(dns_zone_t *zone) {
	dns_dbnode_t *node = NULL;
	dns_dbversion_t *version = NULL;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdataset_t rdataset;
	isc_result_t result;
	dns_db_t *db = NULL;

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL) {
		dns_db_attach(zone->db, &db);
	}
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	if (db == NULL) {
		goto cleanup;
	}

	result = dns_db_findnode(db, &zone->origin, false, &node);
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}

	dns_db_currentversion(db, &version);
	dns_rdataset_init(&rdataset);
	result = dns_db_findrdataset(db, node, version,
				     zone->privatetype,
				     dns_rdatatype_none, 0,
				     &rdataset, NULL);
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto cleanup;
	}

	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset))
	{
		dns_rdataset_current(&rdataset, &rdata);
		if (rdata.length != 5 ||
		    rdata.data[0] == 0 || rdata.data[4] != 0)
		{
			dns_rdata_reset(&rdata);
			continue;
		}

		result = zone_signwithkey(zone, rdata.data[0],
					  (rdata.data[1] << 8) | rdata.data[2],
					  rdata.data[3]);
		if (result != ISC_R_SUCCESS) {
			dnssec_log(zone, ISC_LOG_ERROR,
				   "zone_signwithkey failed: %s",
				   dns_result_totext(result));
		}
		dns_rdata_reset(&rdata);
	}
	dns_rdataset_disassociate(&rdataset);

 cleanup:
	if (db != NULL) {
		if (node != NULL) {
			dns_db_detachnode(db, &node);
		}
		if (version != NULL) {
			dns_db_closeversion(db, &version, false);
		}
		dns_db_detach(&db);
	}
}