dns_zone_verifydb(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *ver) {
	dns_dbversion_t *version = NULL;
	dns_keytable_t *secroots = NULL;
	isc_result_t result;
	dns_name_t *origin;

	const char me[] = "dns_zone_verifydb";
	ENTER;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(db != NULL);

	if (dns_zone_gettype(zone) != dns_zone_mirror) {
		return (ISC_R_SUCCESS);
	}

	if (ver == NULL) {
		dns_db_currentversion(db, &version);
	} else {
		version = ver;
	}

	if (zone->view != NULL) {
		result = dns_view_getsecroots(zone->view, &secroots);
		if (result != ISC_R_SUCCESS) {
			goto done;
		}
	}

	origin = dns_db_origin(db);
	result = dns_zoneverify_dnssec(zone, db, version, origin, secroots,
				       zone->mctx, true, false);

 done:
	if (secroots != NULL) {
		dns_keytable_detach(&secroots);
	}

	if (ver == NULL) {
		dns_db_closeversion(db, &version, false);
	}

	if (result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR, "zone verification failed: %s",
			   isc_result_totext(result));
		result = DNS_R_VERIFYFAILURE;
	}

	return (result);
}