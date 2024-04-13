minimal_update(dns_keyfetch_t *kfetch, dns_dbversion_t *ver, dns_diff_t *diff)
{
	isc_result_t result;
	isc_buffer_t keyb;
	unsigned char key_buf[4096];
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_keydata_t keydata;
	dns_name_t *name;
	dns_zone_t *zone = kfetch->zone;
	isc_stdtime_t now;

	name = dns_fixedname_name(&kfetch->name);
	isc_stdtime_get(&now);

	for (result = dns_rdataset_first(&kfetch->keydataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&kfetch->keydataset)) {
		dns_rdata_reset(&rdata);
		dns_rdataset_current(&kfetch->keydataset, &rdata);

		/* Delete old version */
		CHECK(update_one_rr(kfetch->db, ver, diff, DNS_DIFFOP_DEL,
				    name, 0, &rdata));

		/* Update refresh timer */
		result = dns_rdata_tostruct(&rdata, &keydata, NULL);
		if (result == ISC_R_UNEXPECTEDEND)
			continue;
		if (result != ISC_R_SUCCESS)
			goto failure;
		keydata.refresh = refresh_time(kfetch, true);
		set_refreshkeytimer(zone, &keydata, now, false);

		dns_rdata_reset(&rdata);
		isc_buffer_init(&keyb, key_buf, sizeof(key_buf));
		CHECK(dns_rdata_fromstruct(&rdata,
					   zone->rdclass, dns_rdatatype_keydata,
					   &keydata, &keyb));

		/* Insert updated version */
		CHECK(update_one_rr(kfetch->db, ver, diff, DNS_DIFFOP_ADD,
				    name, 0, &rdata));
	}
	result = ISC_R_SUCCESS;
  failure:
	return (result);
}