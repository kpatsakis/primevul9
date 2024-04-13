dnskey_sane(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *ver,
	    dns_diff_t *diff)
{
	isc_result_t result;
	dns_difftuple_t *tuple;
	bool nseconly = false, nsec3 = false;
	dns_rdatatype_t privatetype = dns_zone_getprivatetype(zone);

	/* Scan the tuples for an NSEC-only DNSKEY */
	for (tuple = ISC_LIST_HEAD(diff->tuples);
	     tuple != NULL;
	     tuple = ISC_LIST_NEXT(tuple, link))
	{
		uint8_t alg;
		if (tuple->rdata.type != dns_rdatatype_dnskey ||
		    tuple->op != DNS_DIFFOP_ADD)
		{
			continue;
		}

		alg = tuple->rdata.data[3];
		if (alg == DST_ALG_RSASHA1) {
			nseconly = true;
			break;
		}
	}

	/* Check existing DB for NSEC-only DNSKEY */
	if (!nseconly) {
		result = dns_nsec_nseconly(db, ver, &nseconly);
		if (result == ISC_R_NOTFOUND) {
			result = ISC_R_SUCCESS;
		}
		CHECK(result);
	}

	/* Check existing DB for NSEC3 */
	if (!nsec3) {
		CHECK(dns_nsec3_activex(db, ver, false,
					privatetype, &nsec3));
	}

	/* Refuse to allow NSEC3 with NSEC-only keys */
	if (nseconly && nsec3) {
		dnssec_log(zone, ISC_LOG_ERROR,
			   "NSEC only DNSKEYs and NSEC3 chains not allowed");
		goto failure;
	}

	return (true);

 failure:
	return (false);
}