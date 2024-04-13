sync_keyzone(dns_zone_t *zone, dns_db_t *db) {
	isc_result_t result = ISC_R_SUCCESS;
	bool changed = false;
	bool commit = false;
	dns_keynode_t *keynode = NULL;
	dns_view_t *view = zone->view;
	dns_keytable_t *sr = NULL;
	dns_dbversion_t *ver = NULL;
	dns_diff_t diff;
	dns_rriterator_t rrit;
	struct addifmissing_arg arg;

	dns_zone_log(zone, ISC_LOG_DEBUG(1), "synchronizing trusted keys");

	dns_diff_init(zone->mctx, &diff);

	CHECK(dns_view_getsecroots(view, &sr));

	result = dns_db_newversion(db, &ver);
	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR,
			   "sync_keyzone:dns_db_newversion -> %s",
			   dns_result_totext(result));
		goto failure;
	}

	/*
	 * Walk the zone DB.  If we find any keys whose names are no longer
	 * in managed-keys (or *are* in trusted-keys, meaning they are
	 * permanent and not RFC5011-maintained), delete them from the
	 * zone.  Otherwise call load_secroots(), which loads keys into
	 * secroots as appropriate.
	 */
	dns_rriterator_init(&rrit, db, ver, 0);
	for (result = dns_rriterator_first(&rrit);
	     result == ISC_R_SUCCESS;
	     result = dns_rriterator_nextrrset(&rrit))
	{
		dns_rdataset_t *rdataset = NULL;
		dns_name_t *rrname = NULL;
		uint32_t ttl;

		dns_rriterator_current(&rrit, &rrname, &ttl, &rdataset, NULL);
		if (!dns_rdataset_isassociated(rdataset)) {
			dns_rriterator_destroy(&rrit);
			goto failure;
		}

		if (rdataset->type != dns_rdatatype_keydata) {
			continue;
		}

		result = dns_keytable_find(sr, rrname, &keynode);
		if ((result != ISC_R_SUCCESS &&
		     result != DNS_R_PARTIALMATCH) ||
		    dns_keynode_managed(keynode) == false)
		{
			CHECK(delete_keydata(db, ver, &diff,
					     rrname, rdataset));
			changed = true;
		} else {
			load_secroots(zone, rrname, rdataset);
		}

		if (keynode != NULL) {
			dns_keytable_detachkeynode(sr, &keynode);
		}
	}
	dns_rriterator_destroy(&rrit);

	/*
	 * Now walk secroots to find any managed keys that aren't
	 * in the zone.  If we find any, we add them to the zone.
	 */
	arg.db = db;
	arg.ver = ver;
	arg.result = ISC_R_SUCCESS;
	arg.diff = &diff;
	arg.zone = zone;
	arg.changed = &changed;
	dns_keytable_forall(sr, addifmissing, &arg);
	result = arg.result;
	if (changed) {
		/* Write changes to journal file. */
		CHECK(update_soa_serial(db, ver, &diff, zone->mctx,
					zone->updatemethod));
		CHECK(zone_journal(zone, &diff, NULL, "sync_keyzone"));

		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_LOADED);
		zone_needdump(zone, 30);
		commit = true;
	}

 failure:
	if (result != ISC_R_SUCCESS &&
	    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED))
	{
		dnssec_log(zone, ISC_LOG_ERROR,
			   "unable to synchronize managed keys: %s",
			   dns_result_totext(result));
		isc_time_settoepoch(&zone->refreshkeytime);
	}
	if (keynode != NULL) {
		dns_keytable_detachkeynode(sr, &keynode);
	}
	if (sr != NULL) {
		dns_keytable_detach(&sr);
	}
	if (ver != NULL) {
		dns_db_closeversion(db, &ver, commit);
	}
	dns_diff_clear(&diff);

	INSIST(ver == NULL);

	return (result);
}