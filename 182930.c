need_nsec_chain(dns_db_t *db, dns_dbversion_t *ver,
		const dns_rdata_nsec3param_t *param,
		bool *answer)
{
	dns_dbnode_t *node = NULL;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_nsec3param_t myparam;
	dns_rdataset_t rdataset;
	isc_result_t result;

	*answer = false;

	result = dns_db_getoriginnode(db, &node);
	RUNTIME_CHECK(result == ISC_R_SUCCESS);

	dns_rdataset_init(&rdataset);

	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_nsec,
				     0, 0, &rdataset, NULL);
	if (result == ISC_R_SUCCESS) {
		dns_rdataset_disassociate(&rdataset);
		dns_db_detachnode(db, &node);
		return (result);
	}
	if (result != ISC_R_NOTFOUND) {
		dns_db_detachnode(db, &node);
		return (result);
	}

	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_nsec3param,
				     0, 0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND) {
		*answer = true;
		dns_db_detachnode(db, &node);
		return (ISC_R_SUCCESS);
	}
	if (result != ISC_R_SUCCESS) {
		dns_db_detachnode(db, &node);
		return (result);
	}

	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset)) {
		dns_rdataset_current(&rdataset, &rdata);
		CHECK(dns_rdata_tostruct(&rdata, &myparam, NULL));
		dns_rdata_reset(&rdata);
		/*
		 * Ignore any NSEC3PARAM removals.
		 */
		if (NSEC3REMOVE(myparam.flags))
			continue;
		/*
		 * Ignore the chain that we are in the process of deleting.
		 */
		if (myparam.hash == param->hash &&
		    myparam.iterations == param->iterations &&
		    myparam.salt_length == param->salt_length &&
		    !memcmp(myparam.salt, param->salt, myparam.salt_length))
			continue;
		/*
		 * Found an active NSEC3 chain.
		 */
		break;
	}
	if (result == ISC_R_NOMORE) {
		*answer = true;
		result = ISC_R_SUCCESS;
	}

 failure:
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	dns_db_detachnode(db, &node);
	return (result);
}