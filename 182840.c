signed_with_key(dns_db_t *db, dns_dbnode_t *node, dns_dbversion_t *version,
		dns_rdatatype_t type, dst_key_t *key)
{
	isc_result_t result;
	dns_rdataset_t rdataset;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_rrsig_t rrsig;

	dns_rdataset_init(&rdataset);
	result = dns_db_findrdataset(db, node, version, dns_rdatatype_rrsig,
				     type, 0, &rdataset, NULL);
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		return (false);
	}
	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset)) {
		dns_rdataset_current(&rdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &rrsig, NULL);
		INSIST(result == ISC_R_SUCCESS);
		if (rrsig.algorithm == dst_key_alg(key) &&
		    rrsig.keyid == dst_key_id(key)) {
			dns_rdataset_disassociate(&rdataset);
			return (true);
		}
		dns_rdata_reset(&rdata);
	}
	dns_rdataset_disassociate(&rdataset);
	return (false);
}