dns__zone_findkeys(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *ver,
		   isc_stdtime_t now, isc_mem_t *mctx, unsigned int maxkeys,
		   dst_key_t **keys, unsigned int *nkeys)
{
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	const char *directory = dns_zone_getkeydirectory(zone);

	CHECK(dns_db_findnode(db, dns_db_origin(db), false, &node));
	memset(keys, 0, sizeof(*keys) * maxkeys);
	result = dns_dnssec_findzonekeys(db, ver, node, dns_db_origin(db),
					 directory, now, mctx, maxkeys, keys,
					 nkeys);
	if (result == ISC_R_NOTFOUND)
		result = ISC_R_SUCCESS;
 failure:
	if (node != NULL)
		dns_db_detachnode(db, &node);
	return (result);
}