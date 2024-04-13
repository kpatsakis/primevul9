load_secroots(dns_zone_t *zone, dns_name_t *name, dns_rdataset_t *rdataset) {
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_keydata_t keydata;
	dns_rdata_dnskey_t dnskey;
	isc_mem_t *mctx = zone->mctx;
	int trusted = 0, revoked = 0, pending = 0;
	isc_stdtime_t now;
	dns_keytable_t *sr = NULL;

	isc_stdtime_get(&now);

	result = dns_view_getsecroots(zone->view, &sr);
	if (result == ISC_R_SUCCESS) {
		dns_keytable_delete(sr, name);
		dns_keytable_detach(&sr);
	}

	/* Now insert all the accepted trust anchors from this keydata set. */
	for (result = dns_rdataset_first(rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(rdataset))
	{
		dns_rdata_reset(&rdata);
		dns_rdataset_current(rdataset, &rdata);

		/* Convert rdata to keydata. */
		result = dns_rdata_tostruct(&rdata, &keydata, NULL);
		if (result == ISC_R_UNEXPECTEDEND) {
			continue;
		}
		RUNTIME_CHECK(result == ISC_R_SUCCESS);

		/* Set the key refresh timer to force a fast refresh. */
		set_refreshkeytimer(zone, &keydata, now, true);

		/* If the removal timer is nonzero, this key was revoked. */
		if (keydata.removehd != 0) {
			revoked++;
			continue;
		}

		/*
		 * If the add timer is still pending, this key is not
		 * trusted yet.
		 */
		if (now < keydata.addhd) {
			pending++;
			continue;
		}

		/* Convert keydata to dnskey. */
		dns_keydata_todnskey(&keydata, &dnskey, NULL);

		/* Add to keytables. */
		trusted++;
		trust_key(zone, name, &dnskey, (keydata.addhd == 0), mctx);
	}

	if (trusted == 0 && pending != 0) {
		char namebuf[DNS_NAME_FORMATSIZE];
		dns_name_format(name, namebuf, sizeof namebuf);
		dnssec_log(zone, ISC_LOG_ERROR,
			   "No valid trust anchors for '%s'!", namebuf);
		dnssec_log(zone, ISC_LOG_ERROR,
			   "%d key(s) revoked, %d still pending",
			     revoked, pending);
		dnssec_log(zone, ISC_LOG_ERROR,
			   "All queries to '%s' will fail", namebuf);
		fail_secure(zone, name);
	}
}