zone_sign(dns_zone_t *zone) {
	const char *me = "zone_sign";
	dns_db_t *db = NULL;
	dns_dbnode_t *node = NULL;
	dns_dbversion_t *version = NULL;
	dns_diff_t _sig_diff;
	dns_diff_t post_diff;
	dns__zonediff_t zonediff;
	dns_fixedname_t fixed;
	dns_fixedname_t nextfixed;
	dns_name_t *name, *nextname;
	dns_rdataset_t rdataset;
	dns_signing_t *signing, *nextsigning;
	dns_signinglist_t cleanup;
	dst_key_t *zone_keys[DNS_MAXZONEKEYS];
	int32_t signatures;
	bool check_ksk, keyset_kskonly, is_ksk;
	bool with_ksk, with_zsk;
	bool commit = false;
	bool is_bottom_of_zone;
	bool build_nsec = false;
	bool build_nsec3 = false;
	bool first;
	isc_result_t result;
	isc_stdtime_t now, inception, soaexpire, expire;
	uint32_t jitter, sigvalidityinterval, expiryinterval;
	unsigned int i, j;
	unsigned int nkeys = 0;
	uint32_t nodes;

	ENTER;

	dns_rdataset_init(&rdataset);
	name = dns_fixedname_initname(&fixed);
	nextname = dns_fixedname_initname(&nextfixed);
	dns_diff_init(zone->mctx, &_sig_diff);
	dns_diff_init(zone->mctx, &post_diff);
	zonediff_init(&zonediff, &_sig_diff);
	ISC_LIST_INIT(cleanup);

	/*
	 * Updates are disabled.  Pause for 1 minute.
	 */
	if (zone->update_disabled) {
		result = ISC_R_FAILURE;
		goto cleanup;
	}

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL) {
		dns_db_attach(zone->db, &db);
	}
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	if (db == NULL) {
		result = ISC_R_FAILURE;
		goto cleanup;
	}

	result = dns_db_newversion(db, &version);
	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR,
			   "zone_sign:dns_db_newversion -> %s",
			   dns_result_totext(result));
		goto cleanup;
	}

	isc_stdtime_get(&now);

	result = dns__zone_findkeys(zone, db, version, now, zone->mctx,
				    DNS_MAXZONEKEYS, zone_keys, &nkeys);
	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR,
			   "zone_sign:dns__zone_findkeys -> %s",
			   dns_result_totext(result));
		goto cleanup;
	}

	sigvalidityinterval = dns_zone_getsigvalidityinterval(zone);
	inception = now - 3600;	/* Allow for clock skew. */
	soaexpire = now + sigvalidityinterval;
	expiryinterval = dns_zone_getsigresigninginterval(zone);
	if (expiryinterval > sigvalidityinterval) {
		expiryinterval = sigvalidityinterval;
	} else {
		expiryinterval = sigvalidityinterval - expiryinterval;
	}

	/*
	 * Spread out signatures over time if they happen to be
	 * clumped.  We don't do this for each add_sigs() call as
	 * we still want some clustering to occur.
	 */
	if (sigvalidityinterval >= 3600U) {
		if (sigvalidityinterval > 7200U) {
			jitter = isc_random_uniform(expiryinterval);
		} else {
			jitter = isc_random_uniform(1200);
		}
		expire = soaexpire - jitter - 1;
	} else {
		expire = soaexpire - 1;
	}

	/*
	 * We keep pulling nodes off each iterator in turn until
	 * we have no more nodes to pull off or we reach the limits
	 * for this quantum.
	 */
	nodes = zone->nodes;
	signatures = zone->signatures;
	signing = ISC_LIST_HEAD(zone->signing);
	first = true;

	check_ksk = DNS_ZONE_OPTION(zone, DNS_ZONEOPT_UPDATECHECKKSK);
	keyset_kskonly = DNS_ZONE_OPTION(zone, DNS_ZONEOPT_DNSKEYKSKONLY);

	/* Determine which type of chain to build */
	CHECK(dns_private_chains(db, version, zone->privatetype,
				 &build_nsec, &build_nsec3));

	/* If neither chain is found, default to NSEC */
	if (!build_nsec && !build_nsec3) {
		build_nsec = true;
	}

	while (signing != NULL && nodes-- > 0 && signatures > 0) {
		bool has_alg = false;
		nextsigning = ISC_LIST_NEXT(signing, link);

		ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
		if (signing->done || signing->db != zone->db) {
			/*
			 * The zone has been reloaded.	We will have
			 * created new signings as part of the reload
			 * process so we can destroy this one.
			 */
			ISC_LIST_UNLINK(zone->signing, signing, link);
			ISC_LIST_APPEND(cleanup, signing, link);
			ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
			goto next_signing;
		}
		ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

		if (signing->db != db) {
			goto next_signing;
		}

		is_bottom_of_zone = false;

		if (first && signing->deleteit) {
			/*
			 * Remove the key we are deleting from consideration.
			 */
			for (i = 0, j = 0; i < nkeys; i++) {
				/*
				 * Find the key we want to remove.
				 */
				if (ALG(zone_keys[i]) == signing->algorithm &&
				    dst_key_id(zone_keys[i]) == signing->keyid)
				{
					if (KSK(zone_keys[i]))
						dst_key_free(&zone_keys[i]);
					continue;
				}
				zone_keys[j] = zone_keys[i];
				j++;
			}
			for (i = j; i < nkeys; i++) {
				zone_keys[i] = NULL;
			}
			nkeys = j;
		}

		dns_dbiterator_current(signing->dbiterator, &node, name);

		if (signing->deleteit) {
			dns_dbiterator_pause(signing->dbiterator);
			CHECK(del_sig(db, version, name, node, nkeys,
				      signing->algorithm, signing->keyid,
				      &has_alg, zonediff.diff));
		}

		/*
		 * On the first pass we need to check if the current node
		 * has not been obscured.
		 */
		if (first) {
			dns_fixedname_t ffound;
			dns_name_t *found;
			found = dns_fixedname_initname(&ffound);
			result = dns_db_find(db, name, version,
					     dns_rdatatype_soa,
					     DNS_DBFIND_NOWILD, 0, NULL, found,
					     NULL, NULL);
			if ((result == DNS_R_DELEGATION ||
			     result == DNS_R_DNAME) &&
			    !dns_name_equal(name, found))
			{
				/*
				 * Remember the obscuring name so that
				 * we skip all obscured names.
				 */
				dns_name_copy(found, name, NULL);
				is_bottom_of_zone = true;
				goto next_node;
			}
		}

		/*
		 * Process one node.
		 */
		with_ksk = false;
		with_zsk = false;
		dns_dbiterator_pause(signing->dbiterator);

		CHECK(check_if_bottom_of_zone(db, node, version,
					      &is_bottom_of_zone));

		for (i = 0; !has_alg && i < nkeys; i++) {
			bool both = false;

			/*
			 * Find the keys we want to sign with.
			 */
			if (!dst_key_isprivate(zone_keys[i])) {
				continue;
			}
			/*
			 * Should be redundant.
			 */
			if (dst_key_inactive(zone_keys[i])) {
				continue;
			}

			/*
			 * When adding look for the specific key.
			 */
			if (!signing->deleteit &&
			    (dst_key_alg(zone_keys[i]) != signing->algorithm ||
			     dst_key_id(zone_keys[i]) != signing->keyid))
			{
				continue;
			}

			/*
			 * When deleting make sure we are properly signed
			 * with the algorithm that was being removed.
			 */
			if (signing->deleteit &&
			    ALG(zone_keys[i]) != signing->algorithm)
			{
				continue;
			}

			/*
			 * Do we do KSK processing?
			 */
			if (check_ksk && !REVOKE(zone_keys[i])) {
				bool have_ksk, have_nonksk;
				if (KSK(zone_keys[i])) {
					have_ksk = true;
					have_nonksk = false;
				} else {
					have_ksk = false;
					have_nonksk = true;
				}
				for (j = 0; j < nkeys; j++) {
					if (j == i ||
					    (ALG(zone_keys[i]) !=
					     ALG(zone_keys[j])))
					{
						continue;
					}
					if (!dst_key_isprivate(zone_keys[j])) {
						continue;
					}
					/*
					 * Should be redundant.
					 */
					if (dst_key_inactive(zone_keys[j])) {
						continue;
					}
					if (REVOKE(zone_keys[j])) {
						continue;
					}
					if (KSK(zone_keys[j])) {
						have_ksk = true;
					} else {
						have_nonksk = true;
					}
					both = have_ksk && have_nonksk;
					if (both) {
						break;
					}
				}
			}
			if (both || REVOKE(zone_keys[i])) {
				is_ksk = KSK(zone_keys[i]);
			} else {
				is_ksk = false;
			}

			/*
			 * If deleting signatures, we need to ensure that
			 * the RRset is still signed at least once by a
			 * KSK and a ZSK.
			 */
			if (signing->deleteit && !is_ksk && with_zsk) {
				continue;
			}

			if (signing->deleteit && is_ksk && with_ksk) {
				continue;
			}

			CHECK(sign_a_node(db, name, node, version, build_nsec3,
					  build_nsec, zone_keys[i], inception,
					  expire, zone->minimum, is_ksk,
					  (both && keyset_kskonly),
					  is_bottom_of_zone, zonediff.diff,
					  &signatures, zone->mctx));
			/*
			 * If we are adding we are done.  Look for other keys
			 * of the same algorithm if deleting.
			 */
			if (!signing->deleteit) {
				break;
			}
			if (!is_ksk) {
				with_zsk = true;
			}
			if (KSK(zone_keys[i])) {
				with_ksk = true;
			}
		}

		/*
		 * Go onto next node.
		 */
 next_node:
		first = false;
		dns_db_detachnode(db, &node);
		do {
			result = dns_dbiterator_next(signing->dbiterator);
			if (result == ISC_R_NOMORE) {
				ISC_LIST_UNLINK(zone->signing, signing, link);
				ISC_LIST_APPEND(cleanup, signing, link);
				dns_dbiterator_pause(signing->dbiterator);
				if (nkeys != 0 && build_nsec) {
					/*
					 * We have finished regenerating the
					 * zone with a zone signing key.
					 * The NSEC chain is now complete and
					 * there is a full set of signatures
					 * for the zone.  We can now clear the
					 * OPT bit from the NSEC record.
					 */
					result = updatesecure(db, version,
							      &zone->origin,
							      zone->minimum,
							      false,
							      &post_diff);
					if (result != ISC_R_SUCCESS) {
						dnssec_log(zone, ISC_LOG_ERROR,
						   "updatesecure -> %s",
						    dns_result_totext(result));
						goto cleanup;
					}
				}
				result = updatesignwithkey(zone, signing,
							   version,
							   build_nsec3,
							   zone->minimum,
							   &post_diff);
				if (result != ISC_R_SUCCESS) {
					dnssec_log(zone, ISC_LOG_ERROR,
						   "updatesignwithkey -> %s",
						   dns_result_totext(result));
					goto cleanup;
				}
				build_nsec = false;
				goto next_signing;
			} else if (result != ISC_R_SUCCESS) {
				dnssec_log(zone, ISC_LOG_ERROR,
					   "zone_sign:"
					   "dns_dbiterator_next -> %s",
					   dns_result_totext(result));
				goto cleanup;
			} else if (is_bottom_of_zone) {
				dns_dbiterator_current(signing->dbiterator,
						       &node, nextname);
				dns_db_detachnode(db, &node);
				if (!dns_name_issubdomain(nextname, name)) {
					break;
				}
			} else {
				break;
			}
		} while (1);
		continue;

 next_signing:
		dns_dbiterator_pause(signing->dbiterator);
		signing = nextsigning;
		first = true;
	}

	if (ISC_LIST_HEAD(post_diff.tuples) != NULL) {
		result = dns__zone_updatesigs(&post_diff, db, version,
					      zone_keys, nkeys, zone,
					      inception, expire, 0, now,
					      check_ksk, keyset_kskonly,
					      &zonediff);
		if (result != ISC_R_SUCCESS) {
			dnssec_log(zone, ISC_LOG_ERROR,
				   "zone_sign:dns__zone_updatesigs -> %s",
				   dns_result_totext(result));
			goto cleanup;
		}
	}

	/*
	 * Have we changed anything?
	 */
	if (ISC_LIST_EMPTY(zonediff.diff->tuples)) {
		if (zonediff.offline) {
			commit = true;
		}
		result = ISC_R_SUCCESS;
		goto pauseall;
	}

	commit = true;

	result = del_sigs(zone, db, version, &zone->origin, dns_rdatatype_soa,
			  &zonediff, zone_keys, nkeys, now, false);
	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR, "zone_sign:del_sigs -> %s",
			   dns_result_totext(result));
		goto cleanup;
	}

	result = update_soa_serial(db, version, zonediff.diff, zone->mctx,
				   zone->updatemethod);
	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR,
			   "zone_sign:update_soa_serial -> %s",
			   dns_result_totext(result));
		goto cleanup;
	}

	/*
	 * Generate maximum life time signatures so that the above loop
	 * termination is sensible.
	 */
	result = add_sigs(db, version, &zone->origin, dns_rdatatype_soa,
			  zonediff.diff, zone_keys, nkeys, zone->mctx,
			  inception, soaexpire, check_ksk, keyset_kskonly);
	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR, "zone_sign:add_sigs -> %s",
			   dns_result_totext(result));
		goto cleanup;
	}

	/*
	 * Write changes to journal file.
	 */
	CHECK(zone_journal(zone, zonediff.diff, NULL, "zone_sign"));

 pauseall:
	/*
	 * Pause all iterators so that dns_db_closeversion() can succeed.
	 */
	for (signing = ISC_LIST_HEAD(zone->signing);
	     signing != NULL;
	     signing = ISC_LIST_NEXT(signing, link))
	{
		dns_dbiterator_pause(signing->dbiterator);
	}

	for (signing = ISC_LIST_HEAD(cleanup);
	     signing != NULL;
	     signing = ISC_LIST_NEXT(signing, link))
	{
		dns_dbiterator_pause(signing->dbiterator);
	}

	/*
	 * Everything has succeeded. Commit the changes.
	 */
	dns_db_closeversion(db, &version, commit);

	/*
	 * Everything succeeded so we can clean these up now.
	 */
	signing = ISC_LIST_HEAD(cleanup);
	while (signing != NULL) {
		ISC_LIST_UNLINK(cleanup, signing, link);
		dns_db_detach(&signing->db);
		dns_dbiterator_destroy(&signing->dbiterator);
		isc_mem_put(zone->mctx, signing, sizeof *signing);
		signing = ISC_LIST_HEAD(cleanup);
	}

	set_resigntime(zone);

	if (commit) {
		LOCK_ZONE(zone);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDNOTIFY);
		zone_needdump(zone, DNS_DUMP_DELAY);
		UNLOCK_ZONE(zone);
	}

 failure:
	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR, "zone_sign: failed: %s",
			   dns_result_totext(result));
	}

 cleanup:
	/*
	 * Pause all dbiterators.
	 */
	for (signing = ISC_LIST_HEAD(zone->signing);
	     signing != NULL;
	     signing = ISC_LIST_NEXT(signing, link))
	{
		dns_dbiterator_pause(signing->dbiterator);
	}

	/*
	 * Rollback the cleanup list.
	 */
	signing = ISC_LIST_HEAD(cleanup);
	while (signing != NULL) {
		ISC_LIST_UNLINK(cleanup, signing, link);
		ISC_LIST_PREPEND(zone->signing, signing, link);
		dns_dbiterator_first(signing->dbiterator);
		dns_dbiterator_pause(signing->dbiterator);
		signing = ISC_LIST_HEAD(cleanup);
	}

	dns_diff_clear(&_sig_diff);

	for (i = 0; i < nkeys; i++) {
		dst_key_free(&zone_keys[i]);
	}

	if (node != NULL) {
		dns_db_detachnode(db, &node);
	}

	if (version != NULL) {
		dns_db_closeversion(db, &version, false);
		dns_db_detach(&db);
	} else if (db != NULL) {
		dns_db_detach(&db);
	}

	if (ISC_LIST_HEAD(zone->signing) != NULL) {
		isc_interval_t interval;
		if (zone->update_disabled || result != ISC_R_SUCCESS) {
			isc_interval_set(&interval, 60, 0);	  /* 1 minute */
		} else {
			isc_interval_set(&interval, 0, 10000000); /* 10 ms */
		}
		isc_time_nowplusinterval(&zone->signingtime, &interval);
	} else {
		isc_time_settoepoch(&zone->signingtime);
	}

	INSIST(version == NULL);
}