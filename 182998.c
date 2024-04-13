delete_nsec(dns_db_t *db, dns_dbversion_t *ver, dns_dbnode_t *node,
	    dns_name_t *name, dns_diff_t *diff)
{
	dns_rdataset_t rdataset;
	isc_result_t result;

	dns_rdataset_init(&rdataset);

	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_nsec,
				     0, 0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND)
		return (ISC_R_SUCCESS);
	if (result != ISC_R_SUCCESS)
		return (result);
	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset)) {
		dns_rdata_t rdata = DNS_RDATA_INIT;

		dns_rdataset_current(&rdataset, &rdata);
		CHECK(update_one_rr(db, ver, diff, DNS_DIFFOP_DEL, name,
				    rdataset.ttl, &rdata));
	}
	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;
 failure:
	dns_rdataset_disassociate(&rdataset);
	return (result);
}