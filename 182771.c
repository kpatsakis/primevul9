del_sigs(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *ver, dns_name_t *name,
	 dns_rdatatype_t type, dns__zonediff_t *zonediff, dst_key_t **keys,
	 unsigned int nkeys, isc_stdtime_t now, bool incremental)
{
	isc_result_t result;
	dns_dbnode_t *node = NULL;
	dns_rdataset_t rdataset;
	unsigned int i;
	dns_rdata_rrsig_t rrsig;
	bool found;
	int64_t timewarn = 0, timemaybe = 0;

	dns_rdataset_init(&rdataset);

	if (type == dns_rdatatype_nsec3)
		result = dns_db_findnsec3node(db, name, false, &node);
	else
		result = dns_db_findnode(db, name, false, &node);
	if (result == ISC_R_NOTFOUND)
		return (ISC_R_SUCCESS);
	if (result != ISC_R_SUCCESS)
		goto failure;
	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_rrsig, type,
				     (isc_stdtime_t) 0, &rdataset, NULL);
	dns_db_detachnode(db, &node);

	if (result == ISC_R_NOTFOUND) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		return (ISC_R_SUCCESS);
	}
	if (result != ISC_R_SUCCESS) {
		INSIST(!dns_rdataset_isassociated(&rdataset));
		goto failure;
	}

	for (result = dns_rdataset_first(&rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(&rdataset)) {
		dns_rdata_t rdata = DNS_RDATA_INIT;

		dns_rdataset_current(&rdataset, &rdata);
		result = dns_rdata_tostruct(&rdata, &rrsig, NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);

		if (type != dns_rdatatype_dnskey) {
			bool warn = false, deleted = false;
			if (delsig_ok(&rrsig, keys, nkeys, &warn)) {
				result = update_one_rr(db, ver, zonediff->diff,
					       DNS_DIFFOP_DELRESIGN, name,
					       rdataset.ttl, &rdata);
				if (result != ISC_R_SUCCESS)
					break;
				deleted = true;
			}
			if (warn) {
				/*
				 * At this point, we've got an RRSIG,
				 * which is signed by an inactive key.
				 * An administrator needs to provide a new
				 * key/alg, but until that time, we want to
				 * keep the old RRSIG.  Marking the key as
				 * offline will prevent us spinning waiting
				 * for the private part.
				 */
				if (incremental && !deleted) {
					result = offline(db, ver, zonediff,
							 name, rdataset.ttl,
							 &rdata);
					if (result != ISC_R_SUCCESS)
						break;
				}

				/*
				 * Log the key id and algorithm of
				 * the inactive key with no replacement
				 */
				if (zone->log_key_expired_timer <= now) {
					char origin[DNS_NAME_FORMATSIZE];
					char algbuf[DNS_NAME_FORMATSIZE];
					dns_name_format(&zone->origin, origin,
							sizeof(origin));
					dns_secalg_format(rrsig.algorithm,
							  algbuf,
							  sizeof(algbuf));
					dns_zone_log(zone, ISC_LOG_WARNING,
						     "Key %s/%s/%d "
						     "missing or inactive "
						     "and has no replacement: "
						     "retaining signatures.",
						     origin, algbuf,
						     rrsig.keyid);
					zone->log_key_expired_timer = now +
									3600;
				}
			}
			continue;
		}

		/*
		 * RRSIG(DNSKEY) requires special processing.
		 */
		found = false;
		for (i = 0; i < nkeys; i++) {
			if (rrsig.algorithm == dst_key_alg(keys[i]) &&
			    rrsig.keyid == dst_key_id(keys[i])) {
				found = true;
				/*
				 * Mark offline RRSIG(DNSKEY).
				 * We want the earliest offline expire time
				 * iff there is a new offline signature.
				 */
				if (!dst_key_inactive(keys[i]) &&
				    !dst_key_isprivate(keys[i]))
				{
					int64_t timeexpire =
					   dns_time64_from32(rrsig.timeexpire);
					if (timewarn != 0 &&
					    timewarn > timeexpire)
						timewarn = timeexpire;
					if (rdata.flags & DNS_RDATA_OFFLINE) {
						if (timemaybe == 0 ||
						    timemaybe > timeexpire)
							timemaybe = timeexpire;
						break;
					}
					if (timewarn == 0)
						timewarn = timemaybe;
					if (timewarn == 0 ||
					    timewarn > timeexpire)
						timewarn = timeexpire;
					result = offline(db, ver, zonediff,
							 name, rdataset.ttl,
							 &rdata);
					break;
				}
				result = update_one_rr(db, ver, zonediff->diff,
						       DNS_DIFFOP_DELRESIGN,
						       name, rdataset.ttl,
						       &rdata);
				break;
			}
		}

		/*
		 * If there is not a matching DNSKEY then
		 * delete the RRSIG.
		 */
		if (!found)
			result = update_one_rr(db, ver, zonediff->diff,
					       DNS_DIFFOP_DELRESIGN, name,
					       rdataset.ttl, &rdata);
		if (result != ISC_R_SUCCESS)
			break;
	}

	dns_rdataset_disassociate(&rdataset);
	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;
	if (timewarn > 0) {
		isc_stdtime_t stdwarn = (isc_stdtime_t)timewarn;
		if (timewarn == stdwarn) {
			set_key_expiry_warning(zone, (isc_stdtime_t)timewarn,
					       now);
		} else {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "key expiry warning time out of range");
		}
	}
 failure:
	if (node != NULL)
		dns_db_detachnode(db, &node);
	return (result);
}