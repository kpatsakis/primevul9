deletematchingnsec3(dns_db_t *db, dns_dbversion_t *ver, dns_dbnode_t *node,
		    dns_name_t *name, const dns_rdata_nsec3param_t *param,
		    dns_diff_t *diff)
{
	dns_rdataset_t rdataset;
	dns_rdata_nsec3_t nsec3;
	isc_result_t result;

	dns_rdataset_init(&rdataset);
	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_nsec3,
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
		CHECK(dns_rdata_tostruct(&rdata, &nsec3, NULL));
		if (nsec3.hash != param->hash ||
		    nsec3.iterations != param->iterations ||
		    nsec3.salt_length != param->salt_length ||
		    memcmp(nsec3.salt, param->salt, nsec3.salt_length))
			continue;
		CHECK(update_one_rr(db, ver, diff, DNS_DIFFOP_DEL, name,
				    rdataset.ttl, &rdata));
	}
	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;
 failure:
	dns_rdataset_disassociate(&rdataset);
	return (result);
}