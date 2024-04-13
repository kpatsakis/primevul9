fixup_nsec3param(dns_db_t *db, dns_dbversion_t *ver, dns_nsec3chain_t *chain,
		 bool active, dns_rdatatype_t privatetype,
		 dns_diff_t *diff)
{
	dns_dbnode_t *node = NULL;
	dns_name_t *name = dns_db_origin(db);
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdataset_t rdataset;
	dns_rdata_nsec3param_t nsec3param;
	isc_result_t result;
	isc_buffer_t buffer;
	unsigned char parambuf[DNS_NSEC3PARAM_BUFFERSIZE];
	dns_ttl_t ttl = 0;
	bool nseconly = false, nsec3ok = false;

	dns_rdataset_init(&rdataset);

	result = dns_db_getoriginnode(db, &node);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_nsec3param,
				     0, 0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND)
		goto try_private;
	if (result != ISC_R_SUCCESS)
		goto failure;

	/*
	 * Preserve the existing ttl.
	 */
	ttl = rdataset.ttl;

	/*
	 * Delete all NSEC3PARAM records which match that in nsec3chain.
	 */
	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset)) {

		dns_rdataset_current(&rdataset, &rdata);
		CHECK(dns_rdata_tostruct(&rdata, &nsec3param, NULL));

		if (nsec3param.hash != chain->nsec3param.hash ||
		    (active && nsec3param.flags != 0) ||
		    nsec3param.iterations != chain->nsec3param.iterations ||
		    nsec3param.salt_length != chain->nsec3param.salt_length ||
		    memcmp(nsec3param.salt, chain->nsec3param.salt,
			   nsec3param.salt_length)) {
			dns_rdata_reset(&rdata);
			continue;
		}

		CHECK(update_one_rr(db, ver, diff, DNS_DIFFOP_DEL,
				    name, rdataset.ttl, &rdata));
		dns_rdata_reset(&rdata);
	}
	if (result != ISC_R_NOMORE)
		goto failure;

	dns_rdataset_disassociate(&rdataset);

 try_private:

	if (active)
		goto add;

	result = dns_nsec_nseconly(db, ver, &nseconly);
	nsec3ok = (result == ISC_R_SUCCESS && !nseconly);

	/*
	 * Delete all private records which match that in nsec3chain.
	 */
	result = dns_db_findrdataset(db, node, ver, privatetype,
				     0, 0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND)
		goto add;
	if (result != ISC_R_SUCCESS)
		goto failure;

	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset)) {
		dns_rdata_t private = DNS_RDATA_INIT;
		unsigned char buf[DNS_NSEC3PARAM_BUFFERSIZE];

		dns_rdataset_current(&rdataset, &private);
		if (!dns_nsec3param_fromprivate(&private, &rdata,
						buf, sizeof(buf)))
			continue;
		CHECK(dns_rdata_tostruct(&rdata, &nsec3param, NULL));

		if ((!nsec3ok &&
		     (nsec3param.flags & DNS_NSEC3FLAG_INITIAL) != 0) ||
		    nsec3param.hash != chain->nsec3param.hash ||
		    nsec3param.iterations != chain->nsec3param.iterations ||
		    nsec3param.salt_length != chain->nsec3param.salt_length ||
		    memcmp(nsec3param.salt, chain->nsec3param.salt,
			   nsec3param.salt_length)) {
			dns_rdata_reset(&rdata);
			continue;
		}

		CHECK(update_one_rr(db, ver, diff, DNS_DIFFOP_DEL,
				    name, rdataset.ttl, &private));
		dns_rdata_reset(&rdata);
	}
	if (result != ISC_R_NOMORE)
		goto failure;

  add:
	if ((chain->nsec3param.flags & DNS_NSEC3FLAG_REMOVE) != 0) {
		result = ISC_R_SUCCESS;
		goto failure;
	}

	/*
	 * Add a NSEC3PARAM record which matches that in nsec3chain but
	 * with all flags bits cleared.
	 *
	 * Note: we do not clear chain->nsec3param.flags as this change
	 * may be reversed.
	 */
	isc_buffer_init(&buffer, &parambuf, sizeof(parambuf));
	CHECK(dns_rdata_fromstruct(&rdata, dns_db_class(db),
				   dns_rdatatype_nsec3param,
				   &chain->nsec3param, &buffer));
	rdata.data[1] = 0;	/* Clear flag bits. */
	CHECK(update_one_rr(db, ver, diff, DNS_DIFFOP_ADD, name, ttl, &rdata));

  failure:
	dns_db_detachnode(db, &node);
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	return (result);
}