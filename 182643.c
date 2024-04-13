sign_a_node(dns_db_t *db, dns_name_t *name, dns_dbnode_t *node,
	    dns_dbversion_t *version, bool build_nsec3,
	    bool build_nsec, dst_key_t *key,
	    isc_stdtime_t inception, isc_stdtime_t expire,
	    unsigned int minimum, bool is_ksk,
	    bool keyset_kskonly, bool is_bottom_of_zone,
	    dns_diff_t *diff, int32_t *signatures, isc_mem_t *mctx)
{
	isc_result_t result;
	dns_rdatasetiter_t *iterator = NULL;
	dns_rdataset_t rdataset;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	isc_buffer_t buffer;
	unsigned char data[1024];
	bool seen_soa, seen_ns, seen_rr, seen_nsec, seen_nsec3, seen_ds;

	result = dns_db_allrdatasets(db, node, version, 0, &iterator);
	if (result != ISC_R_SUCCESS) {
		if (result == ISC_R_NOTFOUND)
			result = ISC_R_SUCCESS;
		return (result);
	}

	dns_rdataset_init(&rdataset);
	isc_buffer_init(&buffer, data, sizeof(data));
	seen_rr = seen_soa = seen_ns = seen_nsec = seen_nsec3 = seen_ds = false;
	for (result = dns_rdatasetiter_first(iterator);
	     result == ISC_R_SUCCESS;
	     result = dns_rdatasetiter_next(iterator)) {
		dns_rdatasetiter_current(iterator, &rdataset);
		if (rdataset.type == dns_rdatatype_soa)
			seen_soa = true;
		else if (rdataset.type == dns_rdatatype_ns)
			seen_ns = true;
		else if (rdataset.type == dns_rdatatype_ds)
			seen_ds = true;
		else if (rdataset.type == dns_rdatatype_nsec)
			seen_nsec = true;
		else if (rdataset.type == dns_rdatatype_nsec3)
			seen_nsec3 = true;
		if (rdataset.type != dns_rdatatype_rrsig)
			seen_rr = true;
		dns_rdataset_disassociate(&rdataset);
	}
	if (result != ISC_R_NOMORE)
		goto failure;
	/*
	 * Going from insecure to NSEC3.
	 * Don't generate NSEC3 records for NSEC3 records.
	 */
	if (build_nsec3 && !seen_nsec3 && seen_rr) {
		bool unsecure = !seen_ds && seen_ns && !seen_soa;
		CHECK(dns_nsec3_addnsec3s(db, version, name, minimum,
					  unsecure, diff));
		(*signatures)--;
	}
	/*
	 * Going from insecure to NSEC.
	 * Don't generate NSEC records for NSEC3 records.
	 */
	if (build_nsec && !seen_nsec3 && !seen_nsec && seen_rr) {
		/*
		 * Build a NSEC record except at the origin.
		 */
		if (!dns_name_equal(name, dns_db_origin(db))) {
			CHECK(add_nsec(db, version, name, node, minimum,
				       is_bottom_of_zone, diff));
			/* Count a NSEC generation as a signature generation. */
			(*signatures)--;
		}
	}
	result = dns_rdatasetiter_first(iterator);
	while (result == ISC_R_SUCCESS) {
		dns_rdatasetiter_current(iterator, &rdataset);
		if (rdataset.type == dns_rdatatype_soa ||
		    rdataset.type == dns_rdatatype_rrsig)
		{
			goto next_rdataset;
		}
		if (rdataset.type == dns_rdatatype_dnskey ||
		    rdataset.type == dns_rdatatype_cdnskey ||
		    rdataset.type == dns_rdatatype_cds)
		{
			/*
			 * CDS and CDNSKEY are signed with KSK like DNSKEY.
			 * (RFC 7344, section 4.1 specifies that they must
			 * be signed with a key in the current DS RRset,
			 * which would only include KSK's.)
			 */
			if (!is_ksk && keyset_kskonly) {
				goto next_rdataset;
			}
		} else if (is_ksk) {
			goto next_rdataset;
		}
		if (seen_ns && !seen_soa &&
		    rdataset.type != dns_rdatatype_ds &&
		    rdataset.type != dns_rdatatype_nsec)
		{
			goto next_rdataset;
		}
		if (signed_with_key(db, node, version, rdataset.type, key)) {
			goto next_rdataset;
		}
		/* Calculate the signature, creating a RRSIG RDATA. */
		isc_buffer_clear(&buffer);
		CHECK(dns_dnssec_sign(name, &rdataset, key, &inception,
				      &expire, mctx, &buffer, &rdata));
		/* Update the database and journal with the RRSIG. */
		/* XXX inefficient - will cause dataset merging */
		CHECK(update_one_rr(db, version, diff, DNS_DIFFOP_ADDRESIGN,
				    name, rdataset.ttl, &rdata));
		dns_rdata_reset(&rdata);
		(*signatures)--;
 next_rdataset:
		dns_rdataset_disassociate(&rdataset);
		result = dns_rdatasetiter_next(iterator);
	}
	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;
 failure:
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	if (iterator != NULL)
		dns_rdatasetiter_destroy(&iterator);
	return (result);
}