zone_get_from_db(dns_zone_t *zone, dns_db_t *db, unsigned int *nscount,
		 unsigned int *soacount, uint32_t *serial,
		 uint32_t *refresh, uint32_t *retry,
		 uint32_t *expire, uint32_t *minimum,
		 unsigned int *errors)
{
	isc_result_t result;
	isc_result_t answer = ISC_R_SUCCESS;
	dns_dbversion_t *version = NULL;
	dns_dbnode_t *node;

	REQUIRE(db != NULL);
	REQUIRE(zone != NULL);

	dns_db_currentversion(db, &version);

	if (nscount != NULL)
		*nscount = 0;
	if (soacount != NULL)
		*soacount = 0;
	if (serial != NULL)
		*serial = 0;
	if (refresh != NULL)
		*refresh = 0;
	if (retry != NULL)
		*retry = 0;
	if (expire != NULL)
		*expire = 0;
	if (errors != NULL)
		*errors = 0;

	node = NULL;
	result = dns_db_findnode(db, &zone->origin, false, &node);
	if (result != ISC_R_SUCCESS) {
		answer = result;
		goto closeversion;
	}

	if (nscount != NULL || errors != NULL) {
		result = zone_count_ns_rr(zone, db, node, version,
					  nscount, errors, true);
		if (result != ISC_R_SUCCESS)
			answer = result;
	}

	if (soacount != NULL || serial != NULL || refresh != NULL
	    || retry != NULL || expire != NULL || minimum != NULL) {
		result = zone_load_soa_rr(db, node, version, soacount,
					  serial, refresh, retry, expire,
					  minimum);
		if (result != ISC_R_SUCCESS)
			answer = result;
	}

	dns_db_detachnode(db, &node);
 closeversion:
	dns_db_closeversion(db, &version, false);

	return (answer);
}