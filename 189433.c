configure_staticstub(const cfg_obj_t *zconfig, dns_zone_t *zone,
		     const char *zname, const char *dbtype)
{
	int i = 0;
	const cfg_obj_t *obj;
	isc_mem_t *mctx = dns_zone_getmctx(zone);
	dns_db_t *db = NULL;
	dns_dbversion_t *dbversion = NULL;
	dns_dbnode_t *apexnode = NULL;
	dns_name_t apexname;
	isc_result_t result;
	dns_rdataset_t rdataset;
	dns_rdatalist_t rdatalist_ns, rdatalist_a, rdatalist_aaaa;
	dns_rdatalist_t* rdatalists[] = {
		&rdatalist_ns, &rdatalist_a, &rdatalist_aaaa, NULL
	};
	dns_rdata_t *rdata;
	isc_region_t region;

	/* Create the DB beforehand */
	RETERR(dns_db_create(mctx, dbtype, dns_zone_getorigin(zone),
			     dns_dbtype_stub, dns_zone_getclass(zone),
			     0, NULL, &db));

	dns_rdataset_init(&rdataset);

	dns_rdatalist_init(&rdatalist_ns);
	rdatalist_ns.rdclass = dns_zone_getclass(zone);
	rdatalist_ns.type = dns_rdatatype_ns;
	rdatalist_ns.ttl = STATICSTUB_SERVER_TTL;

	dns_rdatalist_init(&rdatalist_a);
	rdatalist_a.rdclass = dns_zone_getclass(zone);
	rdatalist_a.type = dns_rdatatype_a;
	rdatalist_a.ttl = STATICSTUB_SERVER_TTL;

	dns_rdatalist_init(&rdatalist_aaaa);
	rdatalist_aaaa.rdclass = dns_zone_getclass(zone);
	rdatalist_aaaa.type = dns_rdatatype_aaaa;
	rdatalist_aaaa.ttl = STATICSTUB_SERVER_TTL;

	/* Prepare zone RRs from the configuration */
	obj = NULL;
	result = cfg_map_get(zconfig, "server-addresses", &obj);
	if (result == ISC_R_SUCCESS) {
		INSIST(obj != NULL);
		CHECK(configure_staticstub_serveraddrs(obj, zone,
						       &rdatalist_ns,
						       &rdatalist_a,
						       &rdatalist_aaaa));
	}

	obj = NULL;
	result = cfg_map_get(zconfig, "server-names", &obj);
	if (result == ISC_R_SUCCESS) {
		INSIST(obj != NULL);
		CHECK(configure_staticstub_servernames(obj, zone,
						       &rdatalist_ns,
						       zname));
	}

	/*
	 * Sanity check: there should be at least one NS RR at the zone apex
	 * to trigger delegation.
	 */
	if (ISC_LIST_EMPTY(rdatalist_ns.rdata)) {
		isc_log_write(ns_g_lctx, NS_LOGCATEGORY_GENERAL,
			      NS_LOGMODULE_SERVER, ISC_LOG_ERROR,
			      "No NS record is configured for a "
			      "static-stub zone '%s'", zname);
		result = ISC_R_FAILURE;
		goto cleanup;
	}

	/*
	 * Now add NS and glue A/AAAA RRsets to the zone DB.
	 * First open a new version for the add operation and get a pointer
	 * to the apex node (all RRs are of the apex name).
	 */
	CHECK(dns_db_newversion(db, &dbversion));

	dns_name_init(&apexname, NULL);
	dns_name_clone(dns_zone_getorigin(zone), &apexname);
	CHECK(dns_db_findnode(db, &apexname, false, &apexnode));

	/* Add NS RRset */
	RUNTIME_CHECK(dns_rdatalist_tordataset(&rdatalist_ns, &rdataset)
		      == ISC_R_SUCCESS);
	CHECK(dns_db_addrdataset(db, apexnode, dbversion, 0, &rdataset,
				 0, NULL));
	dns_rdataset_disassociate(&rdataset);

	/* Add glue A RRset, if any */
	if (!ISC_LIST_EMPTY(rdatalist_a.rdata)) {
		RUNTIME_CHECK(dns_rdatalist_tordataset(&rdatalist_a, &rdataset)
			      == ISC_R_SUCCESS);
		CHECK(dns_db_addrdataset(db, apexnode, dbversion, 0,
					 &rdataset, 0, NULL));
		dns_rdataset_disassociate(&rdataset);
	}

	/* Add glue AAAA RRset, if any */
	if (!ISC_LIST_EMPTY(rdatalist_aaaa.rdata)) {
		RUNTIME_CHECK(dns_rdatalist_tordataset(&rdatalist_aaaa,
						       &rdataset)
			      == ISC_R_SUCCESS);
		CHECK(dns_db_addrdataset(db, apexnode, dbversion, 0,
					 &rdataset, 0, NULL));
		dns_rdataset_disassociate(&rdataset);
	}

	dns_db_closeversion(db, &dbversion, true);
	dns_zone_setdb(zone, db);

	result = ISC_R_SUCCESS;

  cleanup:
	if (dns_rdataset_isassociated(&rdataset)) {
		dns_rdataset_disassociate(&rdataset);
	}
	if (apexnode != NULL) {
		dns_db_detachnode(db, &apexnode);
	}
	if (dbversion != NULL) {
		dns_db_closeversion(db, &dbversion, false);
	}
	if (db != NULL) {
		dns_db_detach(&db);
	}
	for (i = 0; rdatalists[i] != NULL; i++) {
		while ((rdata = ISC_LIST_HEAD(rdatalists[i]->rdata)) != NULL) {
			ISC_LIST_UNLINK(rdatalists[i]->rdata, rdata, link);
			dns_rdata_toregion(rdata, &region);
			isc_mem_put(mctx, rdata,
				    sizeof(*rdata) + region.length);
		}
	}

	INSIST(dbversion == NULL);

	return (result);
}