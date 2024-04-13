dns__zone_updatesigs(dns_diff_t *diff, dns_db_t *db, dns_dbversion_t *version,
		     dst_key_t *zone_keys[], unsigned int nkeys,
		     dns_zone_t *zone, isc_stdtime_t inception,
		     isc_stdtime_t expire, isc_stdtime_t keyexpire,
		     isc_stdtime_t now, bool check_ksk,
		     bool keyset_kskonly, dns__zonediff_t *zonediff)
{
	dns_difftuple_t *tuple;
	isc_result_t result;

	while ((tuple = ISC_LIST_HEAD(diff->tuples)) != NULL) {
		isc_stdtime_t exp = expire;

		if (keyexpire != 0 &&
		    (tuple->rdata.type == dns_rdatatype_dnskey ||
		     tuple->rdata.type == dns_rdatatype_cdnskey ||
		     tuple->rdata.type == dns_rdatatype_cds))
		{
			exp = keyexpire;
		}

		result = del_sigs(zone, db, version, &tuple->name,
				  tuple->rdata.type, zonediff,
				  zone_keys, nkeys, now, false);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "dns__zone_updatesigs:del_sigs -> %s",
				     dns_result_totext(result));
			return (result);
		}
		result = add_sigs(db, version, &tuple->name,
				  tuple->rdata.type, zonediff->diff,
				  zone_keys, nkeys, zone->mctx, inception,
				  exp, check_ksk, keyset_kskonly);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "dns__zone_updatesigs:add_sigs -> %s",
				     dns_result_totext(result));
			return (result);
		}

		/*
		 * Signature changes for all RRs with name tuple->name and type
		 * tuple->rdata.type were appended to zonediff->diff.  Now we
		 * remove all the "raw" changes with the same name and type
		 * from diff (so that they are not processed by this loop
		 * again) and append them to zonediff so that they get applied.
		 */
		move_matching_tuples(tuple, diff, zonediff->diff);
	}
	return (ISC_R_SUCCESS);
}