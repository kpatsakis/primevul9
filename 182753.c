checkandaddsoa(dns_db_t *db, dns_dbnode_t *node, dns_dbversion_t *version,
	       dns_rdataset_t *rdataset, uint32_t oldserial)
{
	dns_rdata_soa_t soa;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdatalist_t temprdatalist;
	dns_rdataset_t temprdataset;
	isc_buffer_t b;
	isc_result_t result;
	unsigned char buf[DNS_SOA_BUFFERSIZE];
	dns_fixedname_t fixed;
	dns_name_t *name;

	result = dns_rdataset_first(rdataset);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	dns_rdataset_current(rdataset, &rdata);
	result = dns_rdata_tostruct(&rdata, &soa, NULL);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	if (isc_serial_gt(soa.serial, oldserial))
		return (dns_db_addrdataset(db, node, version, 0, rdataset, 0,
					   NULL));
	/*
	 * Always bump the serial.
	 */
	oldserial++;
	if (oldserial == 0)
		oldserial++;
	soa.serial = oldserial;

	/*
	 * Construct a replacement rdataset.
	 */
	dns_rdata_reset(&rdata);
	isc_buffer_init(&b, buf, sizeof(buf));
	result = dns_rdata_fromstruct(&rdata, rdataset->rdclass,
				      dns_rdatatype_soa, &soa, &b);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	dns_rdatalist_init(&temprdatalist);
	temprdatalist.rdclass = rdata.rdclass;
	temprdatalist.type = rdata.type;
	temprdatalist.ttl = rdataset->ttl;
	ISC_LIST_APPEND(temprdatalist.rdata, &rdata, link);

	dns_rdataset_init(&temprdataset);
	result = dns_rdatalist_tordataset(&temprdatalist, &temprdataset);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	name = dns_fixedname_initname(&fixed);
	result = dns_db_nodefullname(db, node, name);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	dns_rdataset_getownercase(rdataset, name);
	dns_rdataset_setownercase(&temprdataset, name);
	return (dns_db_addrdataset(db, node, version, 0, &temprdataset,
				   0, NULL));
}