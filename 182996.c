zone_count_ns_rr(dns_zone_t *zone, dns_db_t *db, dns_dbnode_t *node,
		 dns_dbversion_t *version, unsigned int *nscount,
		 unsigned int *errors, bool logit)
{
	isc_result_t result;
	unsigned int count = 0;
	unsigned int ecount = 0;
	dns_rdataset_t rdataset;
	dns_rdata_t rdata;
	dns_rdata_ns_t ns;

	dns_rdataset_init(&rdataset);
	result = dns_db_findrdataset(db, node, version, dns_rdatatype_ns,
				     dns_rdatatype_none, 0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto success;
	}
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto invalidate_rdataset;
	}

	result = dns_rdataset_first(&rdataset);
	while (result == ISC_R_SUCCESS) {
		if (errors != NULL && zone->rdclass == dns_rdataclass_in &&
		    (zone->type == dns_zone_master ||
		     zone->type == dns_zone_slave ||
		     zone->type == dns_zone_mirror))
		{
			dns_rdata_init(&rdata);
			dns_rdataset_current(&rdataset, &rdata);
			result = dns_rdata_tostruct(&rdata, &ns, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			if (dns_name_issubdomain(&ns.name, &zone->origin) &&
			    !zone_check_ns(zone, db, version, &ns.name, logit))
				ecount++;
		}
		count++;
		result = dns_rdataset_next(&rdataset);
	}
	dns_rdataset_disassociate(&rdataset);

 success:
	if (nscount != NULL)
		*nscount = count;
	if (errors != NULL)
		*errors = ecount;

	result = ISC_R_SUCCESS;

 invalidate_rdataset:
	dns_rdataset_invalidate(&rdataset);

	return (result);
}