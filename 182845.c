zone_rekey(dns_zone_t *zone) {
	isc_result_t result;
	dns_db_t *db = NULL;
	dns_dbnode_t *node = NULL;
	dns_dbversion_t *ver = NULL;
	dns_rdataset_t cdsset, soaset, soasigs, keyset, keysigs, cdnskeyset;
	dns_dnsseckeylist_t dnskeys, keys, rmkeys;
	dns_dnsseckey_t *key = NULL;
	dns_diff_t diff, _sig_diff;
	dns__zonediff_t zonediff;
	bool commit = false, newactive = false;
	bool newalg = false;
	bool fullsign;
	dns_ttl_t ttl = 3600;
	const char *dir = NULL;
	isc_mem_t *mctx = NULL;
	isc_stdtime_t now;
	isc_time_t timenow;
	isc_interval_t ival;
	char timebuf[80];

	REQUIRE(DNS_ZONE_VALID(zone));

	ISC_LIST_INIT(dnskeys);
	ISC_LIST_INIT(keys);
	ISC_LIST_INIT(rmkeys);
	dns_rdataset_init(&soaset);
	dns_rdataset_init(&soasigs);
	dns_rdataset_init(&keyset);
	dns_rdataset_init(&keysigs);
	dns_rdataset_init(&cdsset);
	dns_rdataset_init(&cdnskeyset);
	dir = dns_zone_getkeydirectory(zone);
	mctx = zone->mctx;
	dns_diff_init(mctx, &diff);
	dns_diff_init(mctx, &_sig_diff);
	zonediff_init(&zonediff, &_sig_diff);

	CHECK(dns_zone_getdb(zone, &db));
	CHECK(dns_db_newversion(db, &ver));
	CHECK(dns_db_getoriginnode(db, &node));

	TIME_NOW(&timenow);
	now = isc_time_seconds(&timenow);

	dnssec_log(zone, ISC_LOG_INFO, "reconfiguring zone keys");

	/* Get the SOA record's TTL */
	CHECK(dns_db_findrdataset(db, node, ver, dns_rdatatype_soa,
				  dns_rdatatype_none, 0, &soaset, &soasigs));
	ttl = soaset.ttl;
	dns_rdataset_disassociate(&soaset);

	/* Get the DNSKEY rdataset */
	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_dnskey,
				     dns_rdatatype_none, 0, &keyset, &keysigs);
	if (result == ISC_R_SUCCESS) {
		ttl = keyset.ttl;
		CHECK(dns_dnssec_keylistfromrdataset(&zone->origin, dir,
						     mctx, &keyset,
						     &keysigs, &soasigs,
						     false, false,
						     &dnskeys));
	} else if (result != ISC_R_NOTFOUND) {
		goto failure;
	}


	/* Get the CDS rdataset */
	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_cds,
				     dns_rdatatype_none, 0, &cdsset, NULL);
	if (result != ISC_R_SUCCESS && dns_rdataset_isassociated(&cdsset))
		dns_rdataset_disassociate(&cdsset);

	/* Get the CDNSKEY rdataset */
	result = dns_db_findrdataset(db, node, ver, dns_rdatatype_cdnskey,
				     dns_rdatatype_none, 0, &cdnskeyset, NULL);
	if (result != ISC_R_SUCCESS && dns_rdataset_isassociated(&cdnskeyset))
	{
		dns_rdataset_disassociate(&cdnskeyset);
	}

	/*
	 * True when called from "rndc sign".  Indicates the zone should be
	 * fully signed now.
	 */
	fullsign = DNS_ZONEKEY_OPTION(zone, DNS_ZONEKEY_FULLSIGN);

	result = dns_dnssec_findmatchingkeys(&zone->origin, dir, now, mctx,
					     &keys);
	if (result == ISC_R_SUCCESS) {
		bool check_ksk;
		check_ksk = DNS_ZONE_OPTION(zone, DNS_ZONEOPT_UPDATECHECKKSK);

		result = dns_dnssec_updatekeys(&dnskeys, &keys, &rmkeys,
					       &zone->origin, ttl, &diff,
					       !check_ksk, mctx,
					       dnssec_report);
		/*
		 * Keys couldn't be updated for some reason;
		 * try again later.
		 */
		if (result != ISC_R_SUCCESS) {
			dnssec_log(zone, ISC_LOG_ERROR,
				   "zone_rekey:couldn't update zone keys: %s",
				   isc_result_totext(result));
			goto failure;
		}

		/*
		 * Update CDS / CDNSKEY records.
		 */
		result = dns_dnssec_syncupdate(&dnskeys, &rmkeys, &cdsset,
					       &cdnskeyset, now, ttl,
					       &diff, mctx);
		if (result != ISC_R_SUCCESS) {
			dnssec_log(zone, ISC_LOG_ERROR,
				   "zone_rekey:couldn't update CDS/CDNSKEY: %s",
				   isc_result_totext(result));
			goto failure;
		}

		/*
		 * See if any pre-existing keys have newly become active;
		 * also, see if any new key is for a new algorithm, as in that
		 * event, we need to sign the zone fully.  (If there's a new
		 * key, but it's for an already-existing algorithm, then
		 * the zone signing can be handled incrementally.)
		 */
		for (key = ISC_LIST_HEAD(dnskeys);
		     key != NULL;
		     key = ISC_LIST_NEXT(key, link))
		{
			if (!key->first_sign) {
				continue;
			}

			newactive = true;

			if (!dns_rdataset_isassociated(&keysigs)) {
				newalg = true;
				break;
			}

			if (signed_with_alg(&keysigs, dst_key_alg(key->key))) {
				/*
				 * This isn't a new algorithm; clear
				 * first_sign so we won't sign the
				 * whole zone with this key later
				 */
				key->first_sign = false;
			} else {
				newalg = true;
				break;
			}
		}

		if ((newactive || fullsign || !ISC_LIST_EMPTY(diff.tuples)) &&
		    dnskey_sane(zone, db, ver, &diff))
		{
			CHECK(dns_diff_apply(&diff, db, ver));
			CHECK(clean_nsec3param(zone, db, ver, &diff));
			CHECK(add_signing_records(db, zone->privatetype,
						  ver, &diff,
						  (newalg || fullsign)));
			CHECK(update_soa_serial(db, ver, &diff, mctx,
						zone->updatemethod));
			CHECK(add_chains(zone, db, ver, &diff));
			CHECK(sign_apex(zone, db, ver, now, &diff, &zonediff));
			CHECK(zone_journal(zone, zonediff.diff, NULL,
					   "zone_rekey"));
			commit = true;
		}
	}

	dns_db_closeversion(db, &ver, true);

	if (commit) {
		dns_difftuple_t *tuple;

		LOCK_ZONE(zone);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDNOTIFY);

		zone_needdump(zone, DNS_DUMP_DELAY);

		zone_settimer(zone, &timenow);

		/* Remove any signatures from removed keys.  */
		if (!ISC_LIST_EMPTY(rmkeys)) {
			for (key = ISC_LIST_HEAD(rmkeys);
			     key != NULL;
			     key = ISC_LIST_NEXT(key, link))
			{
				result = zone_signwithkey(zone,
							  dst_key_alg(key->key),
							  dst_key_id(key->key),
							  true);
				if (result != ISC_R_SUCCESS) {
					dnssec_log(zone, ISC_LOG_ERROR,
					   "zone_signwithkey failed: %s",
					   dns_result_totext(result));
				}
			}
		}

		if (fullsign) {
			/*
			 * "rndc sign" was called, so we now sign the zone
			 * with all active keys, whether they're new or not.
			 */
			for (key = ISC_LIST_HEAD(dnskeys);
			     key != NULL;
			     key = ISC_LIST_NEXT(key, link))
			{
				if (!key->force_sign && !key->hint_sign) {
					continue;
				}

				result = zone_signwithkey(zone,
							  dst_key_alg(key->key),
							  dst_key_id(key->key),
							  false);
				if (result != ISC_R_SUCCESS) {
					dnssec_log(zone, ISC_LOG_ERROR,
					   "zone_signwithkey failed: %s",
					   dns_result_totext(result));
				}
			}
		} else if (newalg) {
			/*
			 * We haven't been told to sign fully, but a new
			 * algorithm was added to the DNSKEY.  We sign
			 * the full zone, but only with newly active
			 * keys.
			 */
			for (key = ISC_LIST_HEAD(dnskeys);
			     key != NULL;
			     key = ISC_LIST_NEXT(key, link))
			{
				if (!key->first_sign) {
					continue;
				}

				result = zone_signwithkey(zone,
							  dst_key_alg(key->key),
							  dst_key_id(key->key),
							  false);
				if (result != ISC_R_SUCCESS) {
					dnssec_log(zone, ISC_LOG_ERROR,
					   "zone_signwithkey failed: %s",
					   dns_result_totext(result));
				}
			}
		}

		/*
		 * Clear fullsign flag, if it was set, so we don't do
		 * another full signing next time
		 */
		zone->keyopts &= ~DNS_ZONEKEY_FULLSIGN;

		/*
		 * Cause the zone to add/delete NSEC3 chains for the
		 * deferred NSEC3PARAM changes.
		 */
		for (tuple = ISC_LIST_HEAD(zonediff.diff->tuples);
		     tuple != NULL;
		     tuple = ISC_LIST_NEXT(tuple, link))
		{
			unsigned char buf[DNS_NSEC3PARAM_BUFFERSIZE];
			dns_rdata_t rdata = DNS_RDATA_INIT;
			dns_rdata_nsec3param_t nsec3param;

			if (tuple->rdata.type != zone->privatetype ||
			    tuple->op != DNS_DIFFOP_ADD)
			{
				continue;
			}

			if (!dns_nsec3param_fromprivate(&tuple->rdata, &rdata,
							buf, sizeof(buf)))
			{
				continue;
			}

			result = dns_rdata_tostruct(&rdata, &nsec3param, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			if (nsec3param.flags == 0) {
				continue;
			}

			result = zone_addnsec3chain(zone, &nsec3param);
			if (result != ISC_R_SUCCESS) {
				dnssec_log(zone, ISC_LOG_ERROR,
					   "zone_addnsec3chain failed: %s",
					   dns_result_totext(result));
			}
		}

		/*
		 * Activate any NSEC3 chain updates that may have
		 * been scheduled before this rekey.
		 */
		if (fullsign || newalg) {
			resume_addnsec3chain(zone);
		}

		/*
		 * Schedule the next resigning event
		 */
		set_resigntime(zone);
		UNLOCK_ZONE(zone);
	}

	isc_time_settoepoch(&zone->refreshkeytime);

	/*
	 * If we're doing key maintenance, set the key refresh timer to
	 * the next scheduled key event or to 'dnssec-loadkeys-interval'
	 * seconds in the future, whichever is sooner.
	 */
	if (DNS_ZONEKEY_OPTION(zone, DNS_ZONEKEY_MAINTAIN)) {
		isc_time_t timethen;
		isc_stdtime_t then;

		LOCK_ZONE(zone);
		DNS_ZONE_TIME_ADD(&timenow, zone->refreshkeyinterval,
				  &timethen);
		zone->refreshkeytime = timethen;
		UNLOCK_ZONE(zone);

		for (key = ISC_LIST_HEAD(dnskeys);
		     key != NULL;
		     key = ISC_LIST_NEXT(key, link))
		{
			then = now;
			result = next_keyevent(key->key, &then);
			if (result != ISC_R_SUCCESS) {
				continue;
			}

			DNS_ZONE_TIME_ADD(&timenow, then - now, &timethen);
			LOCK_ZONE(zone);
			if (isc_time_compare(&timethen,
					     &zone->refreshkeytime) < 0)
			{
				zone->refreshkeytime = timethen;
			}
			UNLOCK_ZONE(zone);
		}

		zone_settimer(zone, &timenow);

		isc_time_formattimestamp(&zone->refreshkeytime, timebuf, 80);
		dnssec_log(zone, ISC_LOG_INFO, "next key event: %s", timebuf);
	}

	result = ISC_R_SUCCESS;

 failure:
	if (result != ISC_R_SUCCESS) {
		/*
		 * Something went wrong; try again in ten minutes or
		 * after a key refresh interval, whichever is shorter.
		 */
		isc_interval_set(&ival,
				 ISC_MIN(zone->refreshkeyinterval, 600), 0);
		isc_time_nowplusinterval(&zone->refreshkeytime, &ival);
	}

	dns_diff_clear(&diff);
	dns_diff_clear(&_sig_diff);

	clear_keylist(&dnskeys, mctx);
	clear_keylist(&keys, mctx);
	clear_keylist(&rmkeys, mctx);

	if (ver != NULL) {
		dns_db_closeversion(db, &ver, false);
	}
	if (dns_rdataset_isassociated(&cdsset)) {
		dns_rdataset_disassociate(&cdsset);
	}
	if (dns_rdataset_isassociated(&keyset)) {
		dns_rdataset_disassociate(&keyset);
	}
	if (dns_rdataset_isassociated(&keysigs)) {
		dns_rdataset_disassociate(&keysigs);
	}
	if (dns_rdataset_isassociated(&soasigs)) {
		dns_rdataset_disassociate(&soasigs);
	}
	if (dns_rdataset_isassociated(&cdnskeyset)) {
		dns_rdataset_disassociate(&cdnskeyset);
	}
	if (node != NULL) {
		dns_db_detachnode(db, &node);
	}
	if (db != NULL) {
		dns_db_detach(&db);
	}

	INSIST(ver == NULL);
}