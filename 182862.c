del_sig(dns_db_t *db, dns_dbversion_t *version, dns_name_t *name,
	dns_dbnode_t *node, unsigned int nkeys, dns_secalg_t algorithm,
	uint16_t keyid, bool *has_algp, dns_diff_t *diff)
{
	dns_rdata_rrsig_t rrsig;
	dns_rdataset_t rdataset;
	dns_rdatasetiter_t *iterator = NULL;
	isc_result_t result;
	bool alg_missed = false;
	bool alg_found = false;

	char namebuf[DNS_NAME_FORMATSIZE];
	dns_name_format(name, namebuf, sizeof(namebuf));

	result = dns_db_allrdatasets(db, node, version, 0, &iterator);
	if (result != ISC_R_SUCCESS) {
		if (result == ISC_R_NOTFOUND)
			result = ISC_R_SUCCESS;
		return (result);
	}

	dns_rdataset_init(&rdataset);
	for (result = dns_rdatasetiter_first(iterator);
	     result == ISC_R_SUCCESS;
	     result = dns_rdatasetiter_next(iterator)) {
		bool has_alg = false;
		dns_rdatasetiter_current(iterator, &rdataset);
		if (nkeys == 0 && rdataset.type == dns_rdatatype_nsec) {
			for (result = dns_rdataset_first(&rdataset);
			     result == ISC_R_SUCCESS;
			     result = dns_rdataset_next(&rdataset)) {
				dns_rdata_t rdata = DNS_RDATA_INIT;
				dns_rdataset_current(&rdataset, &rdata);
				CHECK(update_one_rr(db, version, diff,
						    DNS_DIFFOP_DEL, name,
						    rdataset.ttl, &rdata));
			}
			if (result != ISC_R_NOMORE)
				goto failure;
			dns_rdataset_disassociate(&rdataset);
			continue;
		}
		if (rdataset.type != dns_rdatatype_rrsig) {
			dns_rdataset_disassociate(&rdataset);
			continue;
		}
		for (result = dns_rdataset_first(&rdataset);
		     result == ISC_R_SUCCESS;
		     result = dns_rdataset_next(&rdataset))
		{
			dns_rdata_t rdata = DNS_RDATA_INIT;
			dns_rdataset_current(&rdataset, &rdata);
			CHECK(dns_rdata_tostruct(&rdata, &rrsig, NULL));
			if (nkeys != 0 &&
			    (rrsig.algorithm != algorithm ||
			     rrsig.keyid != keyid))
			{
				if (rrsig.algorithm == algorithm) {
					has_alg = true;
				}
				continue;
			}
			CHECK(update_one_rr(db, version, diff,
					    DNS_DIFFOP_DELRESIGN, name,
					    rdataset.ttl, &rdata));
		}
		dns_rdataset_disassociate(&rdataset);
		if (result != ISC_R_NOMORE)
			break;

		/*
		 * After deleting, if there's still a signature for
		 * 'algorithm', set alg_found; if not, set alg_missed.
		 */
		if (has_alg) {
			alg_found = true;
		} else {
			alg_missed = true;
		}
	}
	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;

	/*
	 * Set `has_algp` if the algorithm was found in every RRset:
	 * i.e., found in at least one, and not missing from any.
	 */
	*has_algp = (alg_found && !alg_missed);
 failure:
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	dns_rdatasetiter_destroy(&iterator);
	return (result);
}