update_soa_serial(dns_db_t *db, dns_dbversion_t *ver, dns_diff_t *diff,
		  isc_mem_t *mctx, dns_updatemethod_t method) {
	dns_difftuple_t *deltuple = NULL;
	dns_difftuple_t *addtuple = NULL;
	uint32_t serial;
	isc_result_t result;

	INSIST(method != dns_updatemethod_none);

	CHECK(dns_db_createsoatuple(db, ver, mctx, DNS_DIFFOP_DEL, &deltuple));
	CHECK(dns_difftuple_copy(deltuple, &addtuple));
	addtuple->op = DNS_DIFFOP_ADD;

	serial = dns_soa_getserial(&addtuple->rdata);
	serial = dns_update_soaserial(serial, method);
	dns_soa_setserial(serial, &addtuple->rdata);
	CHECK(do_one_tuple(&deltuple, db, ver, diff));
	CHECK(do_one_tuple(&addtuple, db, ver, diff));
	result = ISC_R_SUCCESS;

	failure:
	if (addtuple != NULL)
		dns_difftuple_free(&addtuple);
	if (deltuple != NULL)
		dns_difftuple_free(&deltuple);
	return (result);
}