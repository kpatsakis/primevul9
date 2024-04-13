restore_nsec3param(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *version,
		   nsec3paramlist_t *nsec3list)
{
	isc_result_t result;
	dns_diff_t diff;
	dns_rdata_t rdata;
	nsec3param_t *nsec3p = NULL;
	nsec3param_t *next;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(!ISC_LIST_EMPTY(*nsec3list));

	dns_diff_init(zone->mctx, &diff);

	/*
	 * Loop through the list of private-type records, set the INITIAL
	 * and CREATE flags, and the add the record to the apex of the tree
	 * in db.
	 */
	for (nsec3p = ISC_LIST_HEAD(*nsec3list);
	     nsec3p != NULL;
	     nsec3p = next)
	{
		next = ISC_LIST_NEXT(nsec3p, link);
		dns_rdata_init(&rdata);
		nsec3p->data[2] = DNS_NSEC3FLAG_CREATE | DNS_NSEC3FLAG_INITIAL;
		rdata.length = nsec3p->length;
		rdata.data = nsec3p->data;
		rdata.type = zone->privatetype;
		rdata.rdclass = zone->rdclass;
		result = update_one_rr(db, version, &diff, DNS_DIFFOP_ADD,
				       &zone->origin, 0, &rdata);
		if (result != ISC_R_SUCCESS) {
			break;
		}
	}

	dns_diff_clear(&diff);
	return (result);
}