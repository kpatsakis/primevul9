normalize_key(dns_rdata_t *rr, dns_rdata_t *target,
	      unsigned char *data, int size)
{
	dns_rdata_dnskey_t dnskey;
	dns_rdata_keydata_t keydata;
	isc_buffer_t buf;
	isc_result_t result;

	dns_rdata_reset(target);
	isc_buffer_init(&buf, data, size);

	switch (rr->type) {
	    case dns_rdatatype_dnskey:
		result = dns_rdata_tostruct(rr, &dnskey, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		dnskey.flags &= ~DNS_KEYFLAG_REVOKE;
		dns_rdata_fromstruct(target, rr->rdclass, dns_rdatatype_dnskey,
				     &dnskey, &buf);
		break;
	    case dns_rdatatype_keydata:
		result = dns_rdata_tostruct(rr, &keydata, NULL);
		if (result == ISC_R_UNEXPECTEDEND)
			return (result);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		dns_keydata_todnskey(&keydata, &dnskey, NULL);
		dns_rdata_fromstruct(target, rr->rdclass, dns_rdatatype_dnskey,
				     &dnskey, &buf);
		break;
	    default:
		INSIST(0);
		ISC_UNREACHABLE();
	}
	return (ISC_R_SUCCESS);
}