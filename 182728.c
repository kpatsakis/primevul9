rr_exists(dns_db_t *db, dns_dbversion_t *ver, dns_name_t *name,
	  const dns_rdata_t *rdata, bool *flag)
{
	dns_rdataset_t rdataset;
	dns_dbnode_t *node = NULL;
	isc_result_t result;

	dns_rdataset_init(&rdataset);
	if (rdata->type == dns_rdatatype_nsec3)
		CHECK(dns_db_findnsec3node(db, name, false, &node));
	else
		CHECK(dns_db_findnode(db, name, false, &node));
	result = dns_db_findrdataset(db, node, ver, rdata->type, 0,
				     (isc_stdtime_t) 0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND) {
		*flag = false;
		result = ISC_R_SUCCESS;
		goto failure;
	}

	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset)) {
		dns_rdata_t myrdata = DNS_RDATA_INIT;
		dns_rdataset_current(&rdataset, &myrdata);
		if (!dns_rdata_compare(&myrdata, rdata))
			break;
	}
	dns_rdataset_disassociate(&rdataset);
	if (result == ISC_R_SUCCESS) {
		*flag = true;
	} else if (result == ISC_R_NOMORE) {
		*flag = false;
		result = ISC_R_SUCCESS;
	}

 failure:
	if (node != NULL)
		dns_db_detachnode(db, &node);
	return (result);
}