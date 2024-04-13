resume_addnsec3chain(dns_zone_t *zone) {
	dns_dbnode_t *node = NULL;
	dns_dbversion_t *version = NULL;
	dns_rdataset_t rdataset;
	isc_result_t result;
	dns_rdata_nsec3param_t nsec3param;
	bool nseconly = false, nsec3ok = false;
	dns_db_t *db = NULL;

	INSIST(LOCKED_ZONE(zone));

	if (zone->privatetype == 0)
		return;

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

	/*
	 * In order to create NSEC3 chains we need the DNSKEY RRset at zone
	 * apex to exist and contain no keys using NSEC-only algorithms.
	 */
	result = dns_nsec_nseconly(db, version, &nseconly);
	nsec3ok = (result == ISC_R_SUCCESS && !nseconly);

	/*
	 * Get the RRset containing all private-type records at the zone apex.
	 */
	dns_rdataset_init(&rdataset);
	result = dns_db_findrdataset(db, node, version,
				     zone->privatetype, dns_rdatatype_none,
				     0, &rdataset, NULL);
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto cleanup;
	}

	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset))
	{
		unsigned char buf[DNS_NSEC3PARAM_BUFFERSIZE];
		dns_rdata_t rdata = DNS_RDATA_INIT;
		dns_rdata_t private = DNS_RDATA_INIT;

		dns_rdataset_current(&rdataset, &private);
		/*
		 * Try extracting NSEC3PARAM RDATA from this private-type
		 * record.  Failure means this private-type record does not
		 * represent an NSEC3PARAM record, so skip it.
		 */
		if (!dns_nsec3param_fromprivate(&private, &rdata, buf,
						sizeof(buf)))
		{
			continue;
		}
		result = dns_rdata_tostruct(&rdata, &nsec3param, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		if (((nsec3param.flags & DNS_NSEC3FLAG_REMOVE) != 0) ||
		    ((nsec3param.flags & DNS_NSEC3FLAG_CREATE) != 0 && nsec3ok))
		{
			/*
			 * Pass the NSEC3PARAM RDATA contained in this
			 * private-type record to zone_addnsec3chain() so that
			 * it can kick off adding or removing NSEC3 records.
			 */
			result = zone_addnsec3chain(zone, &nsec3param);
			if (result != ISC_R_SUCCESS) {
				dnssec_log(zone, ISC_LOG_ERROR,
					   "zone_addnsec3chain failed: %s",
					   dns_result_totext(result));
			}
		}
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