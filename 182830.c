refresh_time(dns_keyfetch_t *kfetch, bool retry) {
	isc_result_t result;
	uint32_t t;
	dns_rdataset_t *rdset;
	dns_rdata_t sigrr = DNS_RDATA_INIT;
	dns_rdata_sig_t sig;
	isc_stdtime_t now;

	isc_stdtime_get(&now);

	if (dns_rdataset_isassociated(&kfetch->dnskeysigset))
		rdset = &kfetch->dnskeysigset;
	else
		return (now + dns_zone_mkey_hour);

	result = dns_rdataset_first(rdset);
	if (result != ISC_R_SUCCESS)
		return (now + dns_zone_mkey_hour);

	dns_rdataset_current(rdset, &sigrr);
	result = dns_rdata_tostruct(&sigrr, &sig, NULL);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	if (!retry) {
		t = sig.originalttl / 2;

		if (isc_serial_gt(sig.timeexpire, now)) {
			uint32_t exp = (sig.timeexpire - now) / 2;
			if (t > exp)
				t = exp;
		}

		if (t > (15 * dns_zone_mkey_day))
			t = (15 * dns_zone_mkey_day);

		if (t < dns_zone_mkey_hour)
			t = dns_zone_mkey_hour;
	} else {
		t = sig.originalttl / 10;

		if (isc_serial_gt(sig.timeexpire, now)) {
			uint32_t exp = (sig.timeexpire - now) / 10;
			if (t > exp)
				t = exp;
		}

		if (t > dns_zone_mkey_day)
			t = dns_zone_mkey_day;

		if (t < dns_zone_mkey_hour)
			t = dns_zone_mkey_hour;
	}

	return (now + t);
}