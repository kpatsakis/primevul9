revocable(dns_keyfetch_t *kfetch, dns_rdata_keydata_t *keydata) {
	isc_result_t result;
	dns_name_t *keyname;
	isc_mem_t *mctx;
	dns_rdata_t sigrr = DNS_RDATA_INIT;
	dns_rdata_t rr = DNS_RDATA_INIT;
	dns_rdata_rrsig_t sig;
	dns_rdata_dnskey_t dnskey;
	dst_key_t *dstkey = NULL;
	unsigned char key_buf[4096];
	isc_buffer_t keyb;
	bool answer = false;

	REQUIRE(kfetch != NULL && keydata != NULL);
	REQUIRE(dns_rdataset_isassociated(&kfetch->dnskeysigset));

	keyname = dns_fixedname_name(&kfetch->name);
	mctx = kfetch->zone->view->mctx;

	/* Generate a key from keydata */
	isc_buffer_init(&keyb, key_buf, sizeof(key_buf));
	dns_keydata_todnskey(keydata, &dnskey, NULL);
	dns_rdata_fromstruct(&rr, keydata->common.rdclass,
			     dns_rdatatype_dnskey, &dnskey, &keyb);
	result = dns_dnssec_keyfromrdata(keyname, &rr, mctx, &dstkey);
	if (result != ISC_R_SUCCESS) {
		return (false);
	}

	/* See if that key generated any of the signatures */
	for (result = dns_rdataset_first(&kfetch->dnskeysigset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&kfetch->dnskeysigset))
	{
		dns_fixedname_t fixed;
		dns_fixedname_init(&fixed);

		dns_rdata_reset(&sigrr);
		dns_rdataset_current(&kfetch->dnskeysigset, &sigrr);
		result = dns_rdata_tostruct(&sigrr, &sig, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);

		if (dst_key_alg(dstkey) == sig.algorithm &&
		    dst_key_rid(dstkey) == sig.keyid)
		{
			result = dns_dnssec_verify(keyname,
						   &kfetch->dnskeyset,
						   dstkey, false, 0, mctx,
						   &sigrr,
						   dns_fixedname_name(&fixed));

			dnssec_log(kfetch->zone, ISC_LOG_DEBUG(3),
				   "Confirm revoked DNSKEY is self-signed: %s",
				   dns_result_totext(result));

			if (result == ISC_R_SUCCESS) {
				answer = true;
				break;
			}
		}
	}

	dst_key_free(&dstkey);
	return (answer);
}