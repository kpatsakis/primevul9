check_nsec3param(dns_zone_t *zone, dns_db_t *db) {
	dns_dbnode_t *node = NULL;
	dns_rdataset_t rdataset;
	dns_dbversion_t *version = NULL;
	dns_rdata_nsec3param_t nsec3param;
	bool ok = false;
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	bool dynamic = (zone->type == dns_zone_master)
			? dns_zone_isdynamic(zone, false) : false;

	dns_rdataset_init(&rdataset);
	result = dns_db_findnode(db, &zone->origin, false, &node);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "nsec3param lookup failure: %s",
			     dns_result_totext(result));
		return (result);
	}
	dns_db_currentversion(db, &version);

	result = dns_db_findrdataset(db, node, version,
				     dns_rdatatype_nsec3param,
				     dns_rdatatype_none, 0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		result = ISC_R_SUCCESS;
		goto cleanup;
	}
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "nsec3param lookup failure: %s",
			     dns_result_totext(result));
		goto cleanup;
	}

	/*
	 * For dynamic zones we must support every algorithm so we can
	 * regenerate all the NSEC3 chains.
	 * For non-dynamic zones we only need to find a supported algorithm.
	 */
	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset))
	{
		dns_rdataset_current(&rdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &nsec3param, NULL);
		dns_rdata_reset(&rdata);
		INSIST(result == ISC_R_SUCCESS);
		if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_NSEC3TESTZONE) &&
		    nsec3param.hash == DNS_NSEC3_UNKNOWNALG && !dynamic)
		{
			dns_zone_log(zone, ISC_LOG_WARNING,
			     "nsec3 test \"unknown\" hash algorithm found: %u",
				     nsec3param.hash);
			ok = true;
		} else if (!dns_nsec3_supportedhash(nsec3param.hash)) {
			if (dynamic) {
				dns_zone_log(zone, ISC_LOG_ERROR,
					     "unsupported nsec3 hash algorithm"
					     " in dynamic zone: %u",
					     nsec3param.hash);
				result = DNS_R_BADZONE;
				/* Stop second error message. */
				ok = true;
				break;
			} else
				dns_zone_log(zone, ISC_LOG_WARNING,
				     "unsupported nsec3 hash algorithm: %u",
					     nsec3param.hash);
		} else {
			ok = true;
		}
	}
	if (result == ISC_R_NOMORE) {
		result = ISC_R_SUCCESS;
	}

	if (!ok) {
		result = DNS_R_BADZONE;
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "no supported nsec3 hash algorithm");
	}

 cleanup:
	if (dns_rdataset_isassociated(&rdataset)) {
		dns_rdataset_disassociate(&rdataset);
	}
	dns_db_closeversion(db, &version, false);
	dns_db_detachnode(db, &node);
	return (result);
}