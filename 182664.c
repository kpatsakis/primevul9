set_resigntime(dns_zone_t *zone) {
	dns_rdataset_t rdataset;
	dns_fixedname_t fixed;
	unsigned int resign;
	isc_result_t result;
	uint32_t nanosecs;
	dns_db_t *db = NULL;

	/* We only re-sign zones that can be dynamically updated */
	if (zone->update_disabled)
		return;

	if (!inline_secure(zone) && (zone->type != dns_zone_master ||
	    (zone->ssutable == NULL &&
	     (zone->update_acl == NULL || dns_acl_isnone(zone->update_acl)))))
		return;

	dns_rdataset_init(&rdataset);
	dns_fixedname_init(&fixed);

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL)
		dns_db_attach(zone->db, &db);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	if (db == NULL) {
		isc_time_settoepoch(&zone->resigntime);
		return;
	}

	result = dns_db_getsigningtime(db, &rdataset,
				       dns_fixedname_name(&fixed));
	if (result != ISC_R_SUCCESS) {
		isc_time_settoepoch(&zone->resigntime);
		goto cleanup;
	}

	resign = rdataset.resign - zone->sigresigninginterval;
	dns_rdataset_disassociate(&rdataset);
	nanosecs = isc_random_uniform(1000000000);
	isc_time_set(&zone->resigntime, resign, nanosecs);
 cleanup:
	dns_db_detach(&db);
	return;
}