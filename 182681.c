signed_with_alg(dns_rdataset_t *rdataset, dns_secalg_t alg) {
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_rrsig_t rrsig;
	isc_result_t result;

	REQUIRE(rdataset == NULL || rdataset->type == dns_rdatatype_rrsig);
	if (rdataset == NULL || !dns_rdataset_isassociated(rdataset)) {
		return (false);
	}

	for (result = dns_rdataset_first(rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(rdataset))
	{
		dns_rdataset_current(rdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &rrsig, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		dns_rdata_reset(&rdata);
		if (rrsig.algorithm == alg)
			return (true);
	}

	return (false);
}