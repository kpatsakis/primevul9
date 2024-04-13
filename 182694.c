sync_secure_db(dns_zone_t *seczone, dns_zone_t *raw, dns_db_t *secdb,
	       dns_dbversion_t *secver, dns_difftuple_t **soatuple,
	       dns_diff_t *diff)
{
	isc_result_t result;
	dns_db_t *rawdb = NULL;
	dns_dbversion_t *rawver = NULL;
	dns_difftuple_t *tuple = NULL, *next;
	dns_difftuple_t *oldtuple = NULL, *newtuple = NULL;
	dns_rdata_soa_t oldsoa, newsoa;

	REQUIRE(DNS_ZONE_VALID(seczone));
	REQUIRE(soatuple != NULL && *soatuple == NULL);

	if (!seczone->sourceserialset)
		return (DNS_R_UNCHANGED);

	dns_db_attach(raw->db, &rawdb);
	dns_db_currentversion(rawdb, &rawver);
	result = dns_db_diffx(diff, rawdb, rawver, secdb, secver, NULL);
	dns_db_closeversion(rawdb, &rawver, false);
	dns_db_detach(&rawdb);

	if (result != ISC_R_SUCCESS)
		return (result);

	for (tuple = ISC_LIST_HEAD(diff->tuples);
	     tuple != NULL;
	     tuple = next)
	{
		next = ISC_LIST_NEXT(tuple, link);
		if (tuple->rdata.type == dns_rdatatype_nsec ||
		    tuple->rdata.type == dns_rdatatype_rrsig ||
		    tuple->rdata.type == dns_rdatatype_dnskey ||
		    tuple->rdata.type == dns_rdatatype_nsec3 ||
		    tuple->rdata.type == dns_rdatatype_nsec3param)
		{
			ISC_LIST_UNLINK(diff->tuples, tuple, link);
			dns_difftuple_free(&tuple);
			continue;
		}
		if (tuple->rdata.type == dns_rdatatype_soa) {
			if (tuple->op == DNS_DIFFOP_DEL) {
				INSIST(oldtuple == NULL);
				oldtuple = tuple;
			}
			if (tuple->op == DNS_DIFFOP_ADD) {
				INSIST(newtuple == NULL);
				newtuple = tuple;
			}
		}
	}

	if (oldtuple != NULL && newtuple != NULL) {

		result = dns_rdata_tostruct(&oldtuple->rdata, &oldsoa, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);

		result = dns_rdata_tostruct(&newtuple->rdata, &newsoa, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);

		/*
		 * If the SOA records are the same except for the serial
		 * remove them from the diff.
		 */
		if (oldsoa.refresh == newsoa.refresh &&
		    oldsoa.retry == newsoa.retry &&
		    oldsoa.minimum == newsoa.minimum &&
		    oldsoa.expire == newsoa.expire &&
		    dns_name_equal(&oldsoa.origin, &newsoa.origin) &&
		    dns_name_equal(&oldsoa.contact, &newsoa.contact)) {
			ISC_LIST_UNLINK(diff->tuples, oldtuple, link);
			dns_difftuple_free(&oldtuple);
			ISC_LIST_UNLINK(diff->tuples, newtuple, link);
			dns_difftuple_free(&newtuple);
		}
	}

	if (ISC_LIST_EMPTY(diff->tuples))
		return (DNS_R_UNCHANGED);

	/*
	 * If there are still SOA records in the diff they can now be removed
	 * saving the new SOA record.
	 */
	if (oldtuple != NULL) {
		ISC_LIST_UNLINK(diff->tuples, oldtuple, link);
		dns_difftuple_free(&oldtuple);
	}

	if (newtuple != NULL) {
		ISC_LIST_UNLINK(diff->tuples, newtuple, link);
		*soatuple = newtuple;
	}

	return (ISC_R_SUCCESS);
}