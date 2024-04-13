matchkey(dns_rdataset_t *rdset, dns_rdata_t *rr) {
	unsigned char data1[4096], data2[4096];
	dns_rdata_t rdata, rdata1, rdata2;
	isc_result_t result;

	dns_rdata_init(&rdata);
	dns_rdata_init(&rdata1);
	dns_rdata_init(&rdata2);

	result = normalize_key(rr, &rdata1, data1, sizeof(data1));
	if (result != ISC_R_SUCCESS)
		return (false);

	for (result = dns_rdataset_first(rdset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(rdset)) {
		dns_rdata_reset(&rdata);
		dns_rdataset_current(rdset, &rdata);
		result = normalize_key(&rdata, &rdata2, data2, sizeof(data2));
		if (result != ISC_R_SUCCESS)
			continue;
		if (dns_rdata_compare(&rdata1, &rdata2) == 0)
			return (true);
	}

	return (false);
}