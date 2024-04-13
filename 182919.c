trust_key(dns_zone_t *zone, dns_name_t *keyname,
	  dns_rdata_dnskey_t *dnskey, bool initial,
	  isc_mem_t *mctx)
{
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	unsigned char data[4096];
	isc_buffer_t buffer;
	dns_keytable_t *sr = NULL;
	dst_key_t *dstkey = NULL;

	/* Convert dnskey to DST key. */
	isc_buffer_init(&buffer, data, sizeof(data));
	dns_rdata_fromstruct(&rdata, dnskey->common.rdclass,
			     dns_rdatatype_dnskey, dnskey, &buffer);

	result = dns_view_getsecroots(zone->view, &sr);
	if (result != ISC_R_SUCCESS)
		goto failure;

	CHECK(dns_dnssec_keyfromrdata(keyname, &rdata, mctx, &dstkey));
	CHECK(dns_keytable_add(sr, true, initial, &dstkey));
	dns_keytable_detach(&sr);

  failure:
	if (dstkey != NULL)
		dst_key_free(&dstkey);
	if (sr != NULL)
		dns_keytable_detach(&sr);
	return;
}