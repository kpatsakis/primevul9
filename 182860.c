updatesignwithkey(dns_zone_t *zone, dns_signing_t *signing,
		  dns_dbversion_t *version, bool build_nsec3,
		  dns_ttl_t minimum, dns_diff_t *diff)
{
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	dns_rdataset_t rdataset;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	unsigned char data[5];
	bool seen_done = false;
	bool have_rr = false;

	dns_rdataset_init(&rdataset);
	result = dns_db_getoriginnode(signing->db, &node);
	if (result != ISC_R_SUCCESS)
		goto failure;

	result = dns_db_findrdataset(signing->db, node, version,
				     zone->privatetype, dns_rdatatype_none,
				     0, &rdataset, NULL);
	if (result == ISC_R_NOTFOUND) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		result = ISC_R_SUCCESS;
		goto failure;
	}
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto failure;
	}
	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset)) {
		dns_rdataset_current(&rdataset, &rdata);
		/*
		 * If we don't match the algorithm or keyid skip the record.
		 */
		if (rdata.length != 5 ||
		    rdata.data[0] != signing->algorithm ||
		    rdata.data[1] != ((signing->keyid >> 8) & 0xff) ||
		    rdata.data[2] != (signing->keyid & 0xff)) {
			have_rr = true;
			dns_rdata_reset(&rdata);
			continue;
		}
		/*
		 * We have a match.  If we were signing (!signing->deleteit)
		 * and we already have a record indicating that we have
		 * finished signing (rdata.data[4] != 0) then keep it.
		 * Otherwise it needs to be deleted as we have removed all
		 * the signatures (signing->deleteit), so any record indicating
		 * completion is now out of date, or we have finished signing
		 * with the new record so we no longer need to remember that
		 * we need to sign the zone with the matching key across a
		 * nameserver re-start.
		 */
		if (!signing->deleteit && rdata.data[4] != 0) {
			seen_done = true;
			have_rr = true;
		} else
			CHECK(update_one_rr(signing->db, version, diff,
					    DNS_DIFFOP_DEL, &zone->origin,
					    rdataset.ttl, &rdata));
		dns_rdata_reset(&rdata);
	}
	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;
	if (!signing->deleteit && !seen_done) {
		/*
		 * If we were signing then we need to indicate that we have
		 * finished signing the zone with this key.  If it is already
		 * there we don't need to add it a second time.
		 */
		data[0] = signing->algorithm;
		data[1] = (signing->keyid >> 8) & 0xff;
		data[2] = signing->keyid & 0xff;
		data[3] = 0;
		data[4] = 1;
		rdata.length = sizeof(data);
		rdata.data = data;
		rdata.type = zone->privatetype;
		rdata.rdclass = dns_db_class(signing->db);
		CHECK(update_one_rr(signing->db, version, diff, DNS_DIFFOP_ADD,
				    &zone->origin, rdataset.ttl, &rdata));
	} else if (!have_rr) {
		dns_name_t *origin = dns_db_origin(signing->db);
		/*
		 * Rebuild the NSEC/NSEC3 record for the origin as we no
		 * longer have any private records.
		 */
		if (build_nsec3)
			CHECK(dns_nsec3_addnsec3s(signing->db, version, origin,
						  minimum, false, diff));
		CHECK(updatesecure(signing->db, version, origin, minimum,
				   true, diff));
	}

 failure:
	if (dns_rdataset_isassociated(&rdataset))
		dns_rdataset_disassociate(&rdataset);
	if (node != NULL)
		dns_db_detachnode(signing->db, &node);
	return (result);
}