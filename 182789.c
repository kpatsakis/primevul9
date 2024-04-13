dns_zone_nscheck(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *version,
		 unsigned int *errors)
{
	isc_result_t result;
	dns_dbnode_t *node = NULL;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(errors != NULL);

	result = dns_db_getoriginnode(db, &node);
	if (result != ISC_R_SUCCESS)
		return (result);
	result = zone_count_ns_rr(zone, db, node, version, NULL, errors,
				  false);
	dns_db_detachnode(db, &node);
	return (result);
}