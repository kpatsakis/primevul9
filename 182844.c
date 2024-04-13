zone_unchanged(dns_db_t *db1, dns_db_t *db2, isc_mem_t *mctx) {
	isc_result_t result;
	bool answer = false;
	dns_diff_t diff;

	dns_diff_init(mctx, &diff);
	result = dns_db_diffx(&diff, db1, NULL, db2, NULL, NULL);
	if (result == ISC_R_SUCCESS && ISC_LIST_EMPTY(diff.tuples))
		answer = true;
	dns_diff_clear(&diff);
	return (answer);
}