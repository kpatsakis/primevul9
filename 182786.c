check_if_bottom_of_zone(dns_db_t *db, dns_dbnode_t *node,
			dns_dbversion_t *version, bool *is_bottom_of_zone)
{
	isc_result_t result;
	dns_rdatasetiter_t *iterator = NULL;
	dns_rdataset_t rdataset;
	bool seen_soa = false, seen_ns = false, seen_dname = false;

	REQUIRE(is_bottom_of_zone != NULL);

	result = dns_db_allrdatasets(db, node, version, 0, &iterator);
	if (result != ISC_R_SUCCESS) {
		if (result == ISC_R_NOTFOUND) {
			result = ISC_R_SUCCESS;
		}
		return (result);
	}

	dns_rdataset_init(&rdataset);
	for (result = dns_rdatasetiter_first(iterator);
	     result == ISC_R_SUCCESS;
	     result = dns_rdatasetiter_next(iterator)) {
		dns_rdatasetiter_current(iterator, &rdataset);
		switch (rdataset.type) {
		case dns_rdatatype_soa:
			seen_soa = true;
			break;
		case dns_rdatatype_ns:
			seen_ns = true;
			break;
		case dns_rdatatype_dname:
			seen_dname = true;
			break;
		}
		dns_rdataset_disassociate(&rdataset);
	}
	if (result != ISC_R_NOMORE) {
		goto failure;
	}
	if ((seen_ns && !seen_soa) || seen_dname) {
		*is_bottom_of_zone = true;
	}
	result = ISC_R_SUCCESS;

 failure:
	dns_rdatasetiter_destroy(&iterator);

	return (result);
}