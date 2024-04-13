updatesecure(dns_db_t *db, dns_dbversion_t *version, dns_name_t *name,
	     dns_ttl_t minimum, bool update_only, dns_diff_t *diff)
{
	isc_result_t result;
	dns_rdataset_t rdataset;
	dns_dbnode_t *node = NULL;

	CHECK(dns_db_getoriginnode(db, &node));
	if (update_only) {
		dns_rdataset_init(&rdataset);
		result = dns_db_findrdataset(db, node, version,
					     dns_rdatatype_nsec,
					     dns_rdatatype_none,
					     0, &rdataset, NULL);
		if (dns_rdataset_isassociated(&rdataset))
			dns_rdataset_disassociate(&rdataset);
		if (result == ISC_R_NOTFOUND)
			goto success;
		if (result != ISC_R_SUCCESS)
			goto failure;
	}
	CHECK(delete_nsec(db, version, node, name, diff));
	CHECK(add_nsec(db, version, name, node, minimum, false, diff));
 success:
	result = ISC_R_SUCCESS;
 failure:
	if (node != NULL)
		dns_db_detachnode(db, &node);
	return (result);
}