zone_addnsec3chain(dns_zone_t *zone, dns_rdata_nsec3param_t *nsec3param) {
	dns_nsec3chain_t *nsec3chain, *current;
	dns_dbversion_t *version = NULL;
	bool nseconly = false, nsec3ok = false;
	isc_result_t result;
	isc_time_t now;
	unsigned int options = 0;
	char saltbuf[255*2+1];
	char flags[sizeof("INITIAL|REMOVE|CREATE|NONSEC|OPTOUT")];
	dns_db_t *db = NULL;

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL) {
		dns_db_attach(zone->db, &db);
	}
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

	if (db == NULL) {
		result = ISC_R_SUCCESS;
		goto cleanup;
	}

	/*
	 * If this zone is not NSEC3-capable, attempting to remove any NSEC3
	 * chain from it is pointless as it would not be possible for the
	 * latter to exist in the first place.
	 */
	dns_db_currentversion(db, &version);
	result = dns_nsec_nseconly(db, version, &nseconly);
	nsec3ok = (result == ISC_R_SUCCESS && !nseconly);
	dns_db_closeversion(db, &version, false);
	if (!nsec3ok && (nsec3param->flags & DNS_NSEC3FLAG_REMOVE) == 0) {
		result = ISC_R_SUCCESS;
		goto cleanup;
	}

	/*
	 * Allocate and initialize structure preserving state of
	 * adding/removing records belonging to this NSEC3 chain between
	 * separate zone_nsec3chain() calls.
	 */
	nsec3chain = isc_mem_get(zone->mctx, sizeof *nsec3chain);
	if (nsec3chain == NULL) {
		result = ISC_R_NOMEMORY;
		goto cleanup;
	}

	nsec3chain->magic = 0;
	nsec3chain->done = false;
	nsec3chain->db = NULL;
	nsec3chain->dbiterator = NULL;
	nsec3chain->nsec3param.common.rdclass = nsec3param->common.rdclass;
	nsec3chain->nsec3param.common.rdtype = nsec3param->common.rdtype;
	nsec3chain->nsec3param.hash = nsec3param->hash;
	nsec3chain->nsec3param.iterations = nsec3param->iterations;
	nsec3chain->nsec3param.flags = nsec3param->flags;
	nsec3chain->nsec3param.salt_length = nsec3param->salt_length;
	memmove(nsec3chain->salt, nsec3param->salt, nsec3param->salt_length);
	nsec3chain->nsec3param.salt = nsec3chain->salt;
	nsec3chain->seen_nsec = false;
	nsec3chain->delete_nsec = false;
	nsec3chain->save_delete_nsec = false;

	/*
	 * Log NSEC3 parameters defined by supplied NSEC3PARAM RDATA.
	 */
	if (nsec3param->flags == 0) {
		strlcpy(flags, "NONE", sizeof(flags));
	} else {
		flags[0] = '\0';
		if ((nsec3param->flags & DNS_NSEC3FLAG_REMOVE) != 0) {
			strlcat(flags, "REMOVE", sizeof(flags));
		}
		if ((nsec3param->flags & DNS_NSEC3FLAG_INITIAL) != 0) {
			if (flags[0] == '\0') {
				strlcpy(flags, "INITIAL", sizeof(flags));
			} else {
				strlcat(flags, "|INITIAL", sizeof(flags));
			}
		}
		if ((nsec3param->flags & DNS_NSEC3FLAG_CREATE) != 0) {
			if (flags[0] == '\0') {
				strlcpy(flags, "CREATE", sizeof(flags));
			} else {
				strlcat(flags, "|CREATE", sizeof(flags));
			}
		}
		if ((nsec3param->flags & DNS_NSEC3FLAG_NONSEC) != 0) {
			if (flags[0] == '\0') {
				strlcpy(flags, "NONSEC", sizeof(flags));
			} else {
				strlcat(flags, "|NONSEC", sizeof(flags));
			}
		}
		if ((nsec3param->flags & DNS_NSEC3FLAG_OPTOUT) != 0) {
			if (flags[0] == '\0') {
				strlcpy(flags, "OPTOUT", sizeof(flags));
			} else {
				strlcat(flags, "|OPTOUT", sizeof(flags));
			}
		}
	}
	result = dns_nsec3param_salttotext(nsec3param, saltbuf,
					   sizeof(saltbuf));
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	dnssec_log(zone, ISC_LOG_INFO, "zone_addnsec3chain(%u,%s,%u,%s)",
		   nsec3param->hash, flags, nsec3param->iterations, saltbuf);

	/*
	 * If the NSEC3 chain defined by the supplied NSEC3PARAM RDATA is
	 * currently being processed, interrupt its processing to avoid
	 * simultaneously adding and removing records for the same NSEC3 chain.
	 */
	for (current = ISC_LIST_HEAD(zone->nsec3chain);
	     current != NULL;
	     current = ISC_LIST_NEXT(current, link))
	{
		if ((current->db == db) &&
		    (current->nsec3param.hash == nsec3param->hash) &&
		    (current->nsec3param.iterations ==
		     nsec3param->iterations) &&
		    (current->nsec3param.salt_length ==
		     nsec3param->salt_length) &&
		    memcmp(current->nsec3param.salt, nsec3param->salt,
			   nsec3param->salt_length) == 0)
		{
			current->done = true;
		}
	}

	/*
	 * Attach zone database to the structure initialized above and create
	 * an iterator for it with appropriate options in order to avoid
	 * creating NSEC3 records for NSEC3 records.
	 */
	dns_db_attach(db, &nsec3chain->db);
	if ((nsec3chain->nsec3param.flags & DNS_NSEC3FLAG_CREATE) != 0) {
		options = DNS_DB_NONSEC3;
	}
	result = dns_db_createiterator(nsec3chain->db, options,
				       &nsec3chain->dbiterator);
	if (result == ISC_R_SUCCESS) {
		result = dns_dbiterator_first(nsec3chain->dbiterator);
	}
	if (result == ISC_R_SUCCESS) {
		/*
		 * Database iterator initialization succeeded.  We are now
		 * ready to kick off adding/removing records belonging to this
		 * NSEC3 chain.  Append the structure initialized above to the
		 * "nsec3chain" list for the zone and set the appropriate zone
		 * timer so that zone_nsec3chain() is called as soon as
		 * possible.
		 */
		dns_dbiterator_pause(nsec3chain->dbiterator);
		ISC_LIST_INITANDAPPEND(zone->nsec3chain,
				       nsec3chain, link);
		nsec3chain = NULL;
		if (isc_time_isepoch(&zone->nsec3chaintime)) {
			TIME_NOW(&now);
			zone->nsec3chaintime = now;
			if (zone->task != NULL) {
				zone_settimer(zone, &now);
			}
		}
	}

	if (nsec3chain != NULL) {
		if (nsec3chain->db != NULL) {
			dns_db_detach(&nsec3chain->db);
		}
		if (nsec3chain->dbiterator != NULL) {
			dns_dbiterator_destroy(&nsec3chain->dbiterator);
		}
		isc_mem_put(zone->mctx, nsec3chain, sizeof *nsec3chain);
	}

 cleanup:
	if (db != NULL) {
		dns_db_detach(&db);
	}
	return (result);
}