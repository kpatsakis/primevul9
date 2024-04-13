zone_check_dup(dns_zone_t *zone, dns_db_t *db) {
	dns_dbiterator_t *dbiterator = NULL;
	dns_dbnode_t *node = NULL;
	dns_fixedname_t fixed;
	dns_name_t *name;
	dns_rdataset_t rdataset;
	dns_rdatasetiter_t *rdsit = NULL;
	bool ok = true;
	isc_result_t result;

	name = dns_fixedname_initname(&fixed);
	dns_rdataset_init(&rdataset);

	result = dns_db_createiterator(db, 0, &dbiterator);
	if (result != ISC_R_SUCCESS)
		return (true);

	for (result = dns_dbiterator_first(dbiterator);
	     result == ISC_R_SUCCESS;
	     result = dns_dbiterator_next(dbiterator)) {
		result = dns_dbiterator_current(dbiterator, &node, name);
		if (result != ISC_R_SUCCESS)
			continue;

		result = dns_db_allrdatasets(db, node, NULL, 0, &rdsit);
		if (result != ISC_R_SUCCESS)
			continue;

		for (result = dns_rdatasetiter_first(rdsit);
		     result == ISC_R_SUCCESS;
		     result = dns_rdatasetiter_next(rdsit)) {
			dns_rdatasetiter_current(rdsit, &rdataset);
			if (!zone_rrset_check_dup(zone, name, &rdataset))
				ok = false;
			dns_rdataset_disassociate(&rdataset);
		}
		dns_rdatasetiter_destroy(&rdsit);
		dns_db_detachnode(db, &node);
	}

	if (node != NULL)
		dns_db_detachnode(db, &node);
	dns_dbiterator_destroy(&dbiterator);

	return (ok);
}