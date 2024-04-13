zone_load_soa_rr(dns_db_t *db, dns_dbnode_t *node, dns_dbversion_t *version,
		 unsigned int *soacount,
		 uint32_t *serial, uint32_t *refresh,
		 uint32_t *retry, uint32_t *expire,
		 uint32_t *minimum)
{
	isc_result_t result;
	unsigned int count;
	dns_rdataset_t rdataset;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_soa_t soa;

	dns_rdataset_init(&rdataset);
	result = dns_db_findrdataset(db, node, version, dns_rdatatype_soa,
				     dns_rdatatype_none, 0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		if (soacount != NULL)
			*soacount = 0;
		if (serial != NULL)
			*serial = 0;
		if (refresh != NULL)
			*refresh = 0;
		if (retry != NULL)
			*retry = 0;
		if (expire != NULL)
			*expire = 0;
		if (minimum != NULL)
			*minimum = 0;
		result = ISC_R_SUCCESS;
		goto invalidate_rdataset;
	}
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto invalidate_rdataset;
	}

	count = 0;
	result = dns_rdataset_first(&rdataset);
	while (result == ISC_R_SUCCESS) {
		dns_rdata_init(&rdata);
		dns_rdataset_current(&rdataset, &rdata);
		count++;
		if (count == 1) {
			result = dns_rdata_tostruct(&rdata, &soa, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
		}

		result = dns_rdataset_next(&rdataset);
		dns_rdata_reset(&rdata);
	}
	dns_rdataset_disassociate(&rdataset);

	if (soacount != NULL)
		*soacount = count;

	if (count > 0) {
		if (serial != NULL)
			*serial = soa.serial;
		if (refresh != NULL)
			*refresh = soa.refresh;
		if (retry != NULL)
			*retry = soa.retry;
		if (expire != NULL)
			*expire = soa.expire;
		if (minimum != NULL)
			*minimum = soa.minimum;
	} else {
		if (soacount != NULL)
			*soacount = 0;
		if (serial != NULL)
			*serial = 0;
		if (refresh != NULL)
			*refresh = 0;
		if (retry != NULL)
			*retry = 0;
		if (expire != NULL)
			*expire = 0;
		if (minimum != NULL)
			*minimum = 0;
	}

	result = ISC_R_SUCCESS;

 invalidate_rdataset:
	dns_rdataset_invalidate(&rdataset);

	return (result);
}