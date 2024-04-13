zone_check_dnskeys(dns_zone_t *zone, dns_db_t *db) {
	dns_dbnode_t *node = NULL;
	dns_dbversion_t *version = NULL;
	dns_rdata_dnskey_t dnskey;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdataset_t rdataset;
	isc_result_t result;
	bool logit, foundrsa = false;
	const char *algorithm;

	result = dns_db_findnode(db, &zone->origin, false, &node);
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}

	dns_db_currentversion(db, &version);
	dns_rdataset_init(&rdataset);
	result = dns_db_findrdataset(db, node, version, dns_rdatatype_dnskey,
				     dns_rdatatype_none, 0, &rdataset, NULL);
	if (result != ISC_R_SUCCESS) {
		goto cleanup;
	}

	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset))
	{
		dns_rdataset_current(&rdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &dnskey, NULL);
		INSIST(result == ISC_R_SUCCESS);

		/* RFC 3110, section 4: Performance Considerations:
		 *
		 * A public exponent of 3 minimizes the effort needed to verify
		 * a signature.  Use of 3 as the public exponent is weak for
		 * confidentiality uses since, if the same data can be collected
		 * encrypted under three different keys with an exponent of 3
		 * then, using the Chinese Remainder Theorem [NETSEC], the
		 * original plain text can be easily recovered.  If a key is
		 * known to be used only for authentication, as is the case with
		 * DNSSEC, then an exponent of 3 is acceptable.  However other
		 * applications in the future may wish to leverage DNS
		 * distributed keys for applications that do require
		 * confidentiality.  For keys which might have such other uses,
		 * a more conservative choice would be 65537 (F4, the fourth
		 * fermat number).
		 */
		if (dnskey.algorithm == DST_ALG_RSASHA1 &&
		    dnskey.datalen > 1 && dnskey.data[0] == 1 &&
		    dnskey.data[1] == 3)
		{
			if (dnskey.algorithm == DST_ALG_RSASHA1) {
				logit = !foundrsa;
				foundrsa = true;
				algorithm = "RSASHA1";
			}
			if (logit) {
				dnssec_log(zone, ISC_LOG_WARNING,
					   "weak %s (%u) key found "
					   "(exponent=3)", algorithm,
					   dnskey.algorithm);
			}
		}
		dns_rdata_reset(&rdata);
	}
	dns_rdataset_disassociate(&rdataset);

 cleanup:
	if (node != NULL) {
		dns_db_detachnode(db, &node);
	}
	if (version != NULL) {
		dns_db_closeversion(db, &version, false);
	}
}