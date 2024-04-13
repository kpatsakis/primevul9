clean_nsec3param(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *ver,
		 dns_diff_t *diff)
{
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	dns_rdataset_t rdataset;

	dns_rdataset_init(&rdataset);
	CHECK(dns_db_getoriginnode(db, &node));

	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_dnskey,
				     dns_rdatatype_none, 0, &rdataset, NULL);
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	if (result != ISC_R_NOTFOUND)
		goto failure;

	result = dns_nsec3param_deletechains(db, ver, zone, true, diff);

 failure:
	if (node != NULL)
		dns_db_detachnode(db, &node);
	return (result);
}