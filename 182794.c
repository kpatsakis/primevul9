zone_resigninc(dns_zone_t *zone) {
	const char *me = "zone_resigninc";
	dns_db_t *db = NULL;
	dns_dbversion_t *version = NULL;
	dns_diff_t _sig_diff;
	dns__zonediff_t zonediff;
	dns_fixedname_t fixed;
	dns_name_t *name;
	dns_rdataset_t rdataset;
	dns_rdatatype_t covers;
	dst_key_t *zone_keys[DNS_MAXZONEKEYS];
	bool check_ksk, keyset_kskonly = false;
	isc_result_t result;
	isc_stdtime_t now, inception, soaexpire, expire, stop;
	uint32_t jitter, sigvalidityinterval;
	unsigned int i;
	unsigned int nkeys = 0;
	unsigned int resign;

	ENTER;

	dns_rdataset_init(&rdataset);
	dns_diff_init(zone->mctx, &_sig_diff);
	zonediff_init(&zonediff, &_sig_diff);

	/*
	 * Zone is frozen or automatic resigning is disabled.
	 * Pause for 5 minutes.
	 */
	if (zone->update_disabled ||
	    DNS_ZONEKEY_OPTION(zone, DNS_ZONEKEY_NORESIGN))
	{
		result = ISC_R_FAILURE;
		goto failure;
	}

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	dns_db_attach(zone->db, &db);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

	result = dns_db_newversion(db, &version);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "zone_resigninc:dns_db_newversion -> %s",
			     dns_result_totext(result));
		goto failure;
	}

	isc_stdtime_get(&now);

	result = dns__zone_findkeys(zone, db, version, now, zone->mctx,
				    DNS_MAXZONEKEYS, zone_keys, &nkeys);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "zone_resigninc:dns__zone_findkeys -> %s",
			     dns_result_totext(result));
		goto failure;
	}

	sigvalidityinterval = zone->sigvalidityinterval;
	inception = now - 3600;	/* Allow for clock skew. */
	soaexpire = now + sigvalidityinterval;
	/*
	 * Spread out signatures over time if they happen to be
	 * clumped.  We don't do this for each add_sigs() call as
	 * we still want some clustering to occur.
	 */
	if (sigvalidityinterval >= 3600U) {
		if (sigvalidityinterval > 7200U) {
			jitter = isc_random_uniform(3600);
		} else {
			jitter = isc_random_uniform(1200);
		}
		expire = soaexpire - jitter - 1;
	} else {
		expire = soaexpire - 1;
	}
	stop = now + 5;

	check_ksk = DNS_ZONE_OPTION(zone, DNS_ZONEOPT_UPDATECHECKKSK);
	keyset_kskonly = DNS_ZONE_OPTION(zone, DNS_ZONEOPT_DNSKEYKSKONLY);

	name = dns_fixedname_initname(&fixed);
	result = dns_db_getsigningtime(db, &rdataset, name);
	if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "zone_resigninc:dns_db_getsigningtime -> %s",
			     dns_result_totext(result));
	}

	i = 0;
	while (result == ISC_R_SUCCESS) {
		resign = rdataset.resign - zone->sigresigninginterval;
		covers = rdataset.covers;
		dns_rdataset_disassociate(&rdataset);

		/*
		 * Stop if we hit the SOA as that means we have walked the
		 * entire zone.  The SOA record should always be the most
		 * recent signature.
		 */
		/* XXXMPA increase number of RRsets signed pre call */
		if (covers == dns_rdatatype_soa || i++ > zone->signatures ||
		    resign > stop)
			break;

		result = del_sigs(zone, db, version, name, covers, &zonediff,
				  zone_keys, nkeys, now, true);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "zone_resigninc:del_sigs -> %s",
				     dns_result_totext(result));
			break;
		}

		result = add_sigs(db, version, name, covers, zonediff.diff,
				  zone_keys, nkeys, zone->mctx, inception,
				  expire, check_ksk, keyset_kskonly);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "zone_resigninc:add_sigs -> %s",
				     dns_result_totext(result));
			break;
		}
		result	= dns_db_getsigningtime(db, &rdataset, name);
		if (nkeys == 0 && result == ISC_R_NOTFOUND) {
			result = ISC_R_SUCCESS;
			break;
		}
		if (result != ISC_R_SUCCESS)
			dns_zone_log(zone, ISC_LOG_ERROR,
			     "zone_resigninc:dns_db_getsigningtime -> %s",
				     dns_result_totext(result));
	}

	if (result != ISC_R_NOMORE && result != ISC_R_SUCCESS)
		goto failure;

	result = del_sigs(zone, db, version, &zone->origin, dns_rdatatype_soa,
			  &zonediff, zone_keys, nkeys, now, true);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "zone_resigninc:del_sigs -> %s",
			     dns_result_totext(result));
		goto failure;
	}

	/*
	 * Did we change anything in the zone?
	 */
	if (ISC_LIST_EMPTY(zonediff.diff->tuples)) {
		/*
		 * Commit the changes if any key has been marked as offline.
		 */
		if (zonediff.offline)
			dns_db_closeversion(db, &version, true);
		goto failure;
	}

	/* Increment SOA serial if we have made changes */
	result = update_soa_serial(db, version, zonediff.diff, zone->mctx,
				   zone->updatemethod);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "zone_resigninc:update_soa_serial -> %s",
			     dns_result_totext(result));
		goto failure;
	}

	/*
	 * Generate maximum life time signatures so that the above loop
	 * termination is sensible.
	 */
	result = add_sigs(db, version, &zone->origin, dns_rdatatype_soa,
			  zonediff.diff, zone_keys, nkeys, zone->mctx,
			  inception, soaexpire, check_ksk, keyset_kskonly);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "zone_resigninc:add_sigs -> %s",
			     dns_result_totext(result));
		goto failure;
	}

	/* Write changes to journal file. */
	CHECK(zone_journal(zone, zonediff.diff, NULL, "zone_resigninc"));

	/* Everything has succeeded. Commit the changes. */
	dns_db_closeversion(db, &version, true);

 failure:
	dns_diff_clear(&_sig_diff);
	for (i = 0; i < nkeys; i++)
		dst_key_free(&zone_keys[i]);
	if (version != NULL) {
		dns_db_closeversion(db, &version, false);
		dns_db_detach(&db);
	} else if (db != NULL)
		dns_db_detach(&db);
	if (result == ISC_R_SUCCESS) {
		set_resigntime(zone);
		LOCK_ZONE(zone);
		zone_needdump(zone, DNS_DUMP_DELAY);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDNOTIFY);
		UNLOCK_ZONE(zone);
	} else {
		/*
		 * Something failed.  Retry in 5 minutes.
		 */
		isc_interval_t ival;
		isc_interval_set(&ival, 300, 0);
		isc_time_nowplusinterval(&zone->resigntime, &ival);
	}

	INSIST(version == NULL);
}