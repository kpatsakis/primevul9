add_sigs(dns_db_t *db, dns_dbversion_t *ver, dns_name_t *name,
	 dns_rdatatype_t type, dns_diff_t *diff, dst_key_t **keys,
	 unsigned int nkeys, isc_mem_t *mctx, isc_stdtime_t inception,
	 isc_stdtime_t expire, bool check_ksk,
	 bool keyset_kskonly)
{
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	dns_rdataset_t rdataset;
	dns_rdata_t sig_rdata = DNS_RDATA_INIT;
	unsigned char data[1024]; /* XXX */
	isc_buffer_t buffer;
	unsigned int i, j;

	dns_rdataset_init(&rdataset);
	isc_buffer_init(&buffer, data, sizeof(data));

	if (type == dns_rdatatype_nsec3)
		result = dns_db_findnsec3node(db, name, false, &node);
	else
		result = dns_db_findnode(db, name, false, &node);
	if (result == ISC_R_NOTFOUND)
		return (ISC_R_SUCCESS);
	if (result != ISC_R_SUCCESS)
		goto failure;
	result = dns_db_findrdataset(db, node, ver, type, 0,
				     (isc_stdtime_t) 0, &rdataset, NULL);
	dns_db_detachnode(db, &node);
	if (result == ISC_R_NOTFOUND) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		return (ISC_R_SUCCESS);
	}
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto failure;
	}

	for (i = 0; i < nkeys; i++) {
		bool both = false;

		if (!dst_key_isprivate(keys[i]))
			continue;
		if (dst_key_inactive(keys[i]))	/* Should be redundant. */
			continue;

		if (check_ksk && !REVOKE(keys[i])) {
			bool have_ksk, have_nonksk;
			if (KSK(keys[i])) {
				have_ksk = true;
				have_nonksk = false;
			} else {
				have_ksk = false;
				have_nonksk = true;
			}
			for (j = 0; j < nkeys; j++) {
				if (j == i || ALG(keys[i]) != ALG(keys[j]))
					continue;
				if (!dst_key_isprivate(keys[j]))
					continue;
				if (dst_key_inactive(keys[j]))	/* SBR */
					continue;
				if (REVOKE(keys[j]))
					continue;
				if (KSK(keys[j]))
					have_ksk = true;
				else
					have_nonksk = true;
				both = have_ksk && have_nonksk;
				if (both)
					break;
			}
		}
		if (both) {
			/*
			 * CDS and CDNSKEY are signed with KSK (RFC 7344, 4.1).
			 */
			if (type == dns_rdatatype_dnskey ||
			    type == dns_rdatatype_cdnskey ||
			    type == dns_rdatatype_cds)
			{
				if (!KSK(keys[i]) && keyset_kskonly)
					continue;
			} else if (KSK(keys[i])) {
				continue;
			}
		} else if (REVOKE(keys[i]) && type != dns_rdatatype_dnskey) {
			continue;
		}

		/* Calculate the signature, creating a RRSIG RDATA. */
		isc_buffer_clear(&buffer);
		CHECK(dns_dnssec_sign(name, &rdataset, keys[i],
				      &inception, &expire,
				      mctx, &buffer, &sig_rdata));
		/* Update the database and journal with the RRSIG. */
		/* XXX inefficient - will cause dataset merging */
		CHECK(update_one_rr(db, ver, diff, DNS_DIFFOP_ADDRESIGN,
				    name, rdataset.ttl, &sig_rdata));
		dns_rdata_reset(&sig_rdata);
		isc_buffer_init(&buffer, data, sizeof(data));
	}

 failure:
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	if (node != NULL)
		dns_db_detachnode(db, &node);
	return (result);
}