save_nsrrset(dns_message_t *message, dns_name_t *name,
	     dns_db_t *db, dns_dbversion_t *version)
{
	dns_rdataset_t *nsrdataset = NULL;
	dns_rdataset_t *rdataset = NULL;
	dns_dbnode_t *node = NULL;
	dns_rdata_ns_t ns;
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;

	/*
	 * Extract NS RRset from message.
	 */
	result = dns_message_findname(message, DNS_SECTION_ANSWER, name,
				      dns_rdatatype_ns, dns_rdatatype_none,
				      NULL, &nsrdataset);
	if (result != ISC_R_SUCCESS)
		goto fail;

	/*
	 * Add NS rdataset.
	 */
	result = dns_db_findnode(db, name, true, &node);
	if (result != ISC_R_SUCCESS)
		goto fail;
	result = dns_db_addrdataset(db, node, version, 0,
				    nsrdataset, 0, NULL);
	dns_db_detachnode(db, &node);
	if (result != ISC_R_SUCCESS)
		goto fail;
	/*
	 * Add glue rdatasets.
	 */
	for (result = dns_rdataset_first(nsrdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(nsrdataset)) {
		dns_rdataset_current(nsrdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &ns, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		dns_rdata_reset(&rdata);
		if (!dns_name_issubdomain(&ns.name, name))
			continue;
		rdataset = NULL;
		result = dns_message_findname(message, DNS_SECTION_ADDITIONAL,
					      &ns.name, dns_rdatatype_aaaa,
					      dns_rdatatype_none, NULL,
					      &rdataset);
		if (result == ISC_R_SUCCESS) {
			result = dns_db_findnode(db, &ns.name,
						 true, &node);
			if (result != ISC_R_SUCCESS)
				goto fail;
			result = dns_db_addrdataset(db, node, version, 0,
						    rdataset, 0, NULL);
			dns_db_detachnode(db, &node);
			if (result != ISC_R_SUCCESS)
				goto fail;
		}
		rdataset = NULL;
		result = dns_message_findname(message, DNS_SECTION_ADDITIONAL,
					      &ns.name, dns_rdatatype_a,
					      dns_rdatatype_none, NULL,
					      &rdataset);
		if (result == ISC_R_SUCCESS) {
			result = dns_db_findnode(db, &ns.name,
						 true, &node);
			if (result != ISC_R_SUCCESS)
				goto fail;
			result = dns_db_addrdataset(db, node, version, 0,
						    rdataset, 0, NULL);
			dns_db_detachnode(db, &node);
			if (result != ISC_R_SUCCESS)
				goto fail;
		}
	}
	if (result != ISC_R_NOMORE)
		goto fail;

	return (ISC_R_SUCCESS);

fail:
	return (result);
}