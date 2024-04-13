sign_apex(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *ver,
	  isc_stdtime_t now, dns_diff_t *diff, dns__zonediff_t *zonediff)
{
	isc_result_t result;
	isc_stdtime_t inception, soaexpire, keyexpire;
	bool check_ksk, keyset_kskonly;
	dst_key_t *zone_keys[DNS_MAXZONEKEYS];
	unsigned int nkeys = 0, i;
	dns_difftuple_t *tuple;

	result = dns__zone_findkeys(zone, db, ver, now, zone->mctx,
				    DNS_MAXZONEKEYS, zone_keys, &nkeys);
	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR,
			   "sign_apex:dns__zone_findkeys -> %s",
			   dns_result_totext(result));
		return (result);
	}

	inception = now - 3600;	/* Allow for clock skew. */
	soaexpire = now + dns_zone_getsigvalidityinterval(zone);

	keyexpire = dns_zone_getkeyvalidityinterval(zone);
	if (keyexpire == 0) {
		keyexpire = soaexpire - 1;
	} else {
		keyexpire += now;
	}

	check_ksk = DNS_ZONE_OPTION(zone, DNS_ZONEOPT_UPDATECHECKKSK);
	keyset_kskonly = DNS_ZONE_OPTION(zone, DNS_ZONEOPT_DNSKEYKSKONLY);

	/*
	 * See if dns__zone_updatesigs() will update DNSKEY signature and if
	 * not cause them to sign so that newly activated keys are used.
	 */
	for (tuple = ISC_LIST_HEAD(diff->tuples);
	     tuple != NULL;
	     tuple = ISC_LIST_NEXT(tuple, link))
	{
		if (tuple->rdata.type == dns_rdatatype_dnskey &&
		    dns_name_equal(&tuple->name, &zone->origin))
		{
			break;
		}
	}

	if (tuple == NULL) {
		result = del_sigs(zone, db, ver, &zone->origin,
				  dns_rdatatype_dnskey, zonediff,
				  zone_keys, nkeys, now, false);
		if (result != ISC_R_SUCCESS) {
			dnssec_log(zone, ISC_LOG_ERROR,
				   "sign_apex:del_sigs -> %s",
				   dns_result_totext(result));
			goto failure;
		}
		result = add_sigs(db, ver, &zone->origin, dns_rdatatype_dnskey,
				  zonediff->diff, zone_keys, nkeys, zone->mctx,
				  inception, keyexpire, check_ksk,
				  keyset_kskonly);
		if (result != ISC_R_SUCCESS) {
			dnssec_log(zone, ISC_LOG_ERROR,
				   "sign_apex:add_sigs -> %s",
				   dns_result_totext(result));
			goto failure;
		}
	}

	result = dns__zone_updatesigs(diff, db, ver, zone_keys, nkeys, zone,
				      inception, soaexpire, keyexpire, now,
				      check_ksk, keyset_kskonly, zonediff);

	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR,
			   "sign_apex:dns__zone_updatesigs -> %s",
			   dns_result_totext(result));
		goto failure;
	}

 failure:
	for (i = 0; i < nkeys; i++) {
		dst_key_free(&zone_keys[i]);
	}
	return (result);
}