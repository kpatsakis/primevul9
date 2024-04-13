next_active(dns_db_t *db, dns_dbversion_t *version, dns_name_t *oldname,
	    dns_name_t *newname, bool bottom)
{
	isc_result_t result;
	dns_dbiterator_t *dbit = NULL;
	dns_rdatasetiter_t *rdsit = NULL;
	dns_dbnode_t *node = NULL;

	CHECK(dns_db_createiterator(db, DNS_DB_NONSEC3, &dbit));
	CHECK(dns_dbiterator_seek(dbit, oldname));
	do {
		result = dns_dbiterator_next(dbit);
		if (result == ISC_R_NOMORE)
			CHECK(dns_dbiterator_first(dbit));
		CHECK(dns_dbiterator_current(dbit, &node, newname));
		if (bottom && dns_name_issubdomain(newname, oldname) &&
		    !dns_name_equal(newname, oldname)) {
			dns_db_detachnode(db, &node);
			continue;
		}
		/*
		 * Is this node empty?
		 */
		CHECK(dns_db_allrdatasets(db, node, version, 0, &rdsit));
		result = dns_rdatasetiter_first(rdsit);
		dns_db_detachnode(db, &node);
		dns_rdatasetiter_destroy(&rdsit);
		if (result != ISC_R_NOMORE)
			break;
	} while (1);
 failure:
	if (node != NULL)
		dns_db_detachnode(db, &node);
	if (dbit != NULL)
		dns_dbiterator_destroy(&dbit);
	return (result);
}