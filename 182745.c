zone_refreshkeys(dns_zone_t *zone) {
	const char me[] = "zone_refreshkeys";
	isc_result_t result;
	dns_rriterator_t rrit;
	dns_db_t *db = NULL;
	dns_dbversion_t *ver = NULL;
	dns_diff_t diff;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	dns_rdata_keydata_t kd;
	isc_stdtime_t now;
	bool commit = false;
	bool fetching = false, fetch_err = false;
	bool timerset = false;

	ENTER;
	REQUIRE(zone->db != NULL);

	isc_stdtime_get(&now);

	LOCK_ZONE(zone);
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING)) {
		isc_time_settoepoch(&zone->refreshkeytime);
		UNLOCK_ZONE(zone);
		return;
	}

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	dns_db_attach(zone->db, &db);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

	dns_diff_init(zone->mctx, &diff);

	CHECK(dns_db_newversion(db, &ver));

	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_REFRESHING);

	dns_rriterator_init(&rrit, db, ver, 0);
	for (result = dns_rriterator_first(&rrit);
	     result == ISC_R_SUCCESS;
	     result = dns_rriterator_nextrrset(&rrit))
	{
		isc_stdtime_t timer = 0xffffffff;
		dns_name_t *name = NULL, *kname = NULL;
		dns_rdataset_t *kdset = NULL;
		dns_keyfetch_t *kfetch;
		uint32_t ttl;

		dns_rriterator_current(&rrit, &name, &ttl, &kdset, NULL);
		if (kdset == NULL || kdset->type != dns_rdatatype_keydata ||
		    !dns_rdataset_isassociated(kdset))
		{
			continue;
		}

		/*
		 * Scan the stored keys looking for ones that need
		 * removal or refreshing
		 */
		for (result = dns_rdataset_first(kdset);
		     result == ISC_R_SUCCESS;
		     result = dns_rdataset_next(kdset))
		{
			dns_rdata_reset(&rdata);
			dns_rdataset_current(kdset, &rdata);
			result = dns_rdata_tostruct(&rdata, &kd, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);

			/* Removal timer expired? */
			if (kd.removehd != 0 && kd.removehd < now) {
				CHECK(update_one_rr(db, ver, &diff,
						    DNS_DIFFOP_DEL, name, ttl,
						    &rdata));
				continue;
			}

			/* Acceptance timer expired? */
			if (kd.addhd <= now) {
				timer = kd.addhd;
			}

			/* Or do we just need to refresh the keyset? */
			if (timer > kd.refresh) {
				timer = kd.refresh;
			}

			set_refreshkeytimer(zone, &kd, now, false);
			timerset = true;
		}

		if (timer > now) {
			continue;
		}

		kfetch = isc_mem_get(zone->mctx, sizeof(dns_keyfetch_t));
		if (kfetch == NULL) {
			fetch_err = true;
			goto failure;
		}

		zone->refreshkeycount++;
		kfetch->zone = zone;
		zone->irefs++;
		INSIST(zone->irefs != 0);
		kname = dns_fixedname_initname(&kfetch->name);
		dns_name_dup(name, zone->mctx, kname);
		dns_rdataset_init(&kfetch->dnskeyset);
		dns_rdataset_init(&kfetch->dnskeysigset);
		dns_rdataset_init(&kfetch->keydataset);
		dns_rdataset_clone(kdset, &kfetch->keydataset);
		kfetch->db = NULL;
		dns_db_attach(db, &kfetch->db);
		kfetch->fetch = NULL;

		if (isc_log_wouldlog(dns_lctx, ISC_LOG_DEBUG(3))) {
			char namebuf[DNS_NAME_FORMATSIZE];
			dns_name_format(kname, namebuf,
					sizeof(namebuf));
			dnssec_log(zone, ISC_LOG_DEBUG(3),
				   "Creating key fetch in "
				   "zone_refreshkeys() for '%s'",
				   namebuf);
		}

		/*
		 * Use of DNS_FETCHOPT_NOCACHED is essential here.  If it is
		 * not set and the cache still holds a non-expired, validated
		 * version of the RRset being queried for by the time the
		 * response is received, the cached RRset will be passed to
		 * keyfetch_done() instead of the one received in the response
		 * as the latter will have a lower trust level due to not being
		 * validated until keyfetch_done() is called.
		 */

#ifdef ENABLE_AFL
		if (dns_fuzzing_resolver == false) {
#endif
		result = dns_resolver_createfetch(zone->view->resolver,
						  kname, dns_rdatatype_dnskey,
						  NULL, NULL, NULL, NULL, 0,
						  DNS_FETCHOPT_NOVALIDATE |
						  DNS_FETCHOPT_UNSHARED |
						  DNS_FETCHOPT_NOCACHED,
						  0, NULL, zone->task,
						  keyfetch_done, kfetch,
						  &kfetch->dnskeyset,
						  &kfetch->dnskeysigset,
						  &kfetch->fetch);
#ifdef ENABLE_AFL
		} else {
			result = ISC_R_FAILURE;
		}
#endif
		if (result == ISC_R_SUCCESS) {
			fetching = true;
		} else {
			zone->refreshkeycount--;
			zone->irefs--;
			dns_db_detach(&kfetch->db);
			dns_rdataset_disassociate(&kfetch->keydataset);
			dns_name_free(kname, zone->mctx);
			isc_mem_put(zone->mctx, kfetch, sizeof(dns_keyfetch_t));
			dnssec_log(zone, ISC_LOG_WARNING,
				   "Failed to create fetch for DNSKEY update");
			fetch_err = true;
		}
	}
	if (!ISC_LIST_EMPTY(diff.tuples)) {
		CHECK(update_soa_serial(db, ver, &diff, zone->mctx,
					zone->updatemethod));
		CHECK(zone_journal(zone, &diff, NULL, "zone_refreshkeys"));
		commit = true;
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_LOADED);
		zone_needdump(zone, 30);
	}

  failure:
	if (fetch_err) {
		/*
		 * Error during a key fetch; retry in an hour.
		 */
		isc_time_t timenow, timethen;
		char timebuf[80];

		TIME_NOW(&timenow);
		DNS_ZONE_TIME_ADD(&timenow, dns_zone_mkey_hour, &timethen);
		zone->refreshkeytime = timethen;
		zone_settimer(zone, &timenow);

		isc_time_formattimestamp(&zone->refreshkeytime, timebuf, 80);
		dnssec_log(zone, ISC_LOG_DEBUG(1), "retry key refresh: %s",
			   timebuf);
	} else if (!timerset) {
		isc_time_settoepoch(&zone->refreshkeytime);
	}

	if (!fetching) {
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_REFRESHING);
	}

	dns_diff_clear(&diff);
	if (ver != NULL) {
		dns_rriterator_destroy(&rrit);
		dns_db_closeversion(db, &ver, commit);
	}
	dns_db_detach(&db);

	UNLOCK_ZONE(zone);

	INSIST(ver == NULL);
}