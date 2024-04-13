dns_zone_cdscheck(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *version) {
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	dns_rdataset_t dnskey, cds, cdnskey;
	unsigned char buffer[DNS_DS_BUFFERSIZE];
	unsigned char algorithms[256];
	unsigned int i;

	REQUIRE(DNS_ZONE_VALID(zone));

	result = dns_db_getoriginnode(db, &node);
	if (result != ISC_R_SUCCESS)
		return (result);

	dns_rdataset_init(&cds);
	dns_rdataset_init(&dnskey);
	dns_rdataset_init(&cdnskey);

	result = dns_db_findrdataset(db, node, version, dns_rdatatype_cds,
				     dns_rdatatype_none, 0, &cds, NULL);
	if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND)
		goto failure;

	result = dns_db_findrdataset(db, node, version, dns_rdatatype_cdnskey,
				     dns_rdatatype_none, 0, &cdnskey, NULL);
	if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND)
		goto failure;

	if (!dns_rdataset_isassociated(&cds) &&
	    !dns_rdataset_isassociated(&cdnskey)) {
		result = ISC_R_SUCCESS;
		goto failure;
	}

	result = dns_db_findrdataset(db, node, version, dns_rdatatype_dnskey,
				     dns_rdatatype_none, 0, &dnskey, NULL);
	if (result == ISC_R_NOTFOUND) {
		if (dns_rdataset_isassociated(&cds))
			result = DNS_R_BADCDS;
		else
			result = DNS_R_BADCDNSKEY;
		goto failure;
	}
	if (result != ISC_R_SUCCESS)
		goto failure;

	/*
	 * For each DNSSEC algorithm in the CDS RRset there must be
	 * a matching DNSKEY record.
	 */
	if (dns_rdataset_isassociated(&cds)) {
		memset(algorithms, 0, sizeof(algorithms));
		for (result = dns_rdataset_first(&cds);
		     result == ISC_R_SUCCESS;
		     result = dns_rdataset_next(&cds)) {
			dns_rdata_t crdata = DNS_RDATA_INIT;
			dns_rdata_cds_t structcds;

			dns_rdataset_current(&cds, &crdata);
			CHECK(dns_rdata_tostruct(&crdata, &structcds, NULL));
			if (algorithms[structcds.algorithm] == 0)
				algorithms[structcds.algorithm] = 1;
			for (result = dns_rdataset_first(&dnskey);
			     result == ISC_R_SUCCESS;
			     result = dns_rdataset_next(&dnskey)) {
				dns_rdata_t rdata = DNS_RDATA_INIT;
				dns_rdata_t dsrdata = DNS_RDATA_INIT;

				dns_rdataset_current(&dnskey, &rdata);
				CHECK(dns_ds_buildrdata(&zone->origin, &rdata,
							structcds.digest_type,
							buffer, &dsrdata));
				if (crdata.length == dsrdata.length &&
				    memcmp(crdata.data, dsrdata.data,
					   dsrdata.length) == 0) {
					algorithms[structcds.algorithm] = 2;
				}
			}
			if (result != ISC_R_NOMORE)
				goto failure;
		}
		for (i = 0; i < sizeof(algorithms); i++) {
			if (algorithms[i] == 1) {
				result = DNS_R_BADCDNSKEY;
				goto failure;
			}
		}
	}

	/*
	 * For each DNSSEC algorithm in the CDNSKEY RRset there must be
	 * a matching DNSKEY record.
	 */
	if (dns_rdataset_isassociated(&cdnskey)) {
		memset(algorithms, 0, sizeof(algorithms));
		for (result = dns_rdataset_first(&cdnskey);
		     result == ISC_R_SUCCESS;
		     result = dns_rdataset_next(&cdnskey)) {
			dns_rdata_t crdata = DNS_RDATA_INIT;
			dns_rdata_cdnskey_t structcdnskey;

			dns_rdataset_current(&cdnskey, &crdata);
			CHECK(dns_rdata_tostruct(&crdata, &structcdnskey,
						 NULL));
			if (algorithms[structcdnskey.algorithm] == 0)
				algorithms[structcdnskey.algorithm] = 1;
			for (result = dns_rdataset_first(&dnskey);
			     result == ISC_R_SUCCESS;
			     result = dns_rdataset_next(&dnskey)) {
				dns_rdata_t rdata = DNS_RDATA_INIT;

				dns_rdataset_current(&dnskey, &rdata);
				if (crdata.length == rdata.length &&
				    memcmp(crdata.data, rdata.data,
					   rdata.length) == 0) {
					algorithms[structcdnskey.algorithm] = 2;
				}
			}
			if (result != ISC_R_NOMORE)
				goto failure;
		}
		for (i = 0; i < sizeof(algorithms); i++) {
			if (algorithms[i] == 1) {
				result = DNS_R_BADCDS;
				goto failure;
			}
		}
	}
	result = ISC_R_SUCCESS;

 failure:
	if (dns_rdataset_isassociated(&cds))
		dns_rdataset_disassociate(&cds);
	if (dns_rdataset_isassociated(&dnskey))
		dns_rdataset_disassociate(&dnskey);
	if (dns_rdataset_isassociated(&cdnskey))
		dns_rdataset_disassociate(&cdnskey);
	dns_db_detachnode(db, &node);
	return (result);
}