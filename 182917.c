add_soa(dns_zone_t *zone, dns_db_t *db) {
	isc_result_t result;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	unsigned char buf[DNS_SOA_BUFFERSIZE];
	dns_dbversion_t *ver = NULL;
	dns_diff_t diff;

	dns_zone_log(zone, ISC_LOG_DEBUG(1), "creating SOA");

	dns_diff_init(zone->mctx, &diff);
	result = dns_db_newversion(db, &ver);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "add_soa:dns_db_newversion -> %s",
			     dns_result_totext(result));
		goto failure;
	}

	/* Build SOA record */
	result = dns_soa_buildrdata(&zone->origin, dns_rootname, zone->rdclass,
				    0, 0, 0, 0, 0, buf, &rdata);
	if (result != ISC_R_SUCCESS) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "add_soa:dns_soa_buildrdata -> %s",
			     dns_result_totext(result));
		goto failure;
	}

	result = update_one_rr(db, ver, &diff, DNS_DIFFOP_ADD,
			       &zone->origin, 0, &rdata);

failure:
	dns_diff_clear(&diff);
	if (ver != NULL)
		dns_db_closeversion(db, &ver, (result == ISC_R_SUCCESS));

	INSIST(ver == NULL);

	return (result);
}