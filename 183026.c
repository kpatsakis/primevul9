save_nsec3param(dns_zone_t *zone, nsec3paramlist_t *nsec3list) {
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	dns_rdataset_t rdataset, prdataset;
	dns_dbversion_t *version = NULL;
	nsec3param_t *nsec3param = NULL;
	nsec3param_t *nsec3p = NULL;
	nsec3param_t *next;
	dns_db_t *db = NULL;
	unsigned char buf[DNS_NSEC3PARAM_BUFFERSIZE];

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(nsec3list != NULL);
	REQUIRE(ISC_LIST_EMPTY(*nsec3list));

	dns_rdataset_init(&rdataset);
	dns_rdataset_init(&prdataset);

	dns_db_attach(zone->db, &db);
	CHECK(dns_db_getoriginnode(db, &node));

	dns_db_currentversion(db, &version);
	result = dns_db_findrdataset(db, node, version,
				     dns_rdatatype_nsec3param,
				     dns_rdatatype_none, 0, &rdataset, NULL);

	if (result != ISC_R_SUCCESS)
		goto getprivate;

	/*
	 * walk nsec3param rdataset making a list of parameters (note that
	 * multiple simultaneous nsec3 chains are annoyingly legal -- this
	 * is why we use an nsec3list, even tho we will usually only have
	 * one)
	 */
	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset))
	{
		dns_rdata_t rdata = DNS_RDATA_INIT;
		dns_rdata_t private = DNS_RDATA_INIT;

		dns_rdataset_current(&rdataset, &rdata);
		isc_log_write(dns_lctx, DNS_LOGCATEGORY_GENERAL,
			      DNS_LOGMODULE_ZONE, ISC_LOG_DEBUG(3),
			      "looping through nsec3param data");
		nsec3param = isc_mem_get(zone->mctx, sizeof(nsec3param_t));
		if (nsec3param == NULL)
			CHECK(ISC_R_NOMEMORY);
		ISC_LINK_INIT(nsec3param, link);

		/*
		 * now transfer the data from the rdata to
		 * the nsec3param
		 */
		dns_nsec3param_toprivate(&rdata, &private,
					 zone->privatetype, nsec3param->data,
					 sizeof(nsec3param->data));
		nsec3param->length = private.length;
		ISC_LIST_APPEND(*nsec3list, nsec3param, link);
	}

 getprivate:
	result = dns_db_findrdataset(db, node, version, zone->privatetype,
				     dns_rdatatype_none, 0, &prdataset, NULL);
	if (result != ISC_R_SUCCESS)
		goto done;

	/*
	 * walk private type records, converting them to nsec3 parameters
	 * using dns_nsec3param_fromprivate(), do the right thing based on
	 * CREATE and REMOVE flags
	 */
	for (result = dns_rdataset_first(&prdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&prdataset))
	{
		dns_rdata_t rdata = DNS_RDATA_INIT;
		dns_rdata_t private = DNS_RDATA_INIT;

		dns_rdataset_current(&prdataset, &private);
		isc_log_write(dns_lctx, DNS_LOGCATEGORY_GENERAL,
			      DNS_LOGMODULE_ZONE, ISC_LOG_DEBUG(3),
			      "looping through nsec3param private data");

		/*
		 * Do we have a valid private record?
		 */
		if (!dns_nsec3param_fromprivate(&private, &rdata,
						buf, sizeof(buf)))
			continue;

		/*
		 * Remove any NSEC3PARAM records scheduled to be removed.
		 */
		if (NSEC3REMOVE(rdata.data[1])) {
			/*
			 * Zero out the flags.
			 */
			rdata.data[1] = 0;

			for (nsec3p = ISC_LIST_HEAD(*nsec3list);
			     nsec3p != NULL;
			     nsec3p = next)
			{
				next = ISC_LIST_NEXT(nsec3p, link);

				if (nsec3p->length == rdata.length + 1 &&
				    memcmp(rdata.data, nsec3p->data + 1,
					   nsec3p->length - 1) == 0) {
					ISC_LIST_UNLINK(*nsec3list,
							nsec3p, link);
					isc_mem_put(zone->mctx, nsec3p,
						    sizeof(nsec3param_t));
				}
			}
			continue;
		}

		nsec3param = isc_mem_get(zone->mctx, sizeof(nsec3param_t));
		if (nsec3param == NULL)
			CHECK(ISC_R_NOMEMORY);
		ISC_LINK_INIT(nsec3param, link);

		/*
		 * Copy the remaining private records so the nsec/nsec3
		 * chain gets created.
		 */
		INSIST(private.length <= sizeof(nsec3param->data));
		memmove(nsec3param->data, private.data, private.length);
		nsec3param->length = private.length;
		ISC_LIST_APPEND(*nsec3list, nsec3param, link);
	}

 done:
	if (result == ISC_R_NOMORE || result == ISC_R_NOTFOUND)
		result = ISC_R_SUCCESS;

 failure:
	if (node != NULL)
		dns_db_detachnode(db, &node);
	if (version != NULL)
		dns_db_closeversion(db, &version, false);
	if (db != NULL)
		dns_db_detach(&db);
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	if (dns_rdataset_isassociated(&prdataset))
		dns_rdataset_disassociate(&prdataset);
	return (result);
}