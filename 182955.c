zone_rrset_check_dup(dns_zone_t *zone, dns_name_t *owner,
		     dns_rdataset_t *rdataset)
{
	dns_rdataset_t tmprdataset;
	isc_result_t result;
	bool answer = true;
	bool format = true;
	int level = ISC_LOG_WARNING;
	char ownerbuf[DNS_NAME_FORMATSIZE];
	char typebuf[DNS_RDATATYPE_FORMATSIZE];
	unsigned int count1 = 0;

	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKDUPRRFAIL))
		level = ISC_LOG_ERROR;

	dns_rdataset_init(&tmprdataset);
	for (result = dns_rdataset_first(rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(rdataset)) {
		dns_rdata_t rdata1 = DNS_RDATA_INIT;
		unsigned int count2 = 0;

		count1++;
		dns_rdataset_current(rdataset, &rdata1);
		dns_rdataset_clone(rdataset, &tmprdataset);
		for (result = dns_rdataset_first(&tmprdataset);
		     result == ISC_R_SUCCESS;
		     result = dns_rdataset_next(&tmprdataset)) {
			dns_rdata_t rdata2 = DNS_RDATA_INIT;
			count2++;
			if (count1 >= count2)
				continue;
			dns_rdataset_current(&tmprdataset, &rdata2);
			if (dns_rdata_casecompare(&rdata1, &rdata2) == 0) {
				if (format) {
					dns_name_format(owner, ownerbuf,
							sizeof ownerbuf);
					dns_rdatatype_format(rdata1.type,
							     typebuf,
							     sizeof(typebuf));
					format = false;
				}
				dns_zone_log(zone, level, "%s/%s has "
					     "semantically identical records",
					     ownerbuf, typebuf);
				if (level == ISC_LOG_ERROR)
					answer = false;
				break;
			}
		}
		dns_rdataset_disassociate(&tmprdataset);
		if (!format)
			break;
	}
	return (answer);
}