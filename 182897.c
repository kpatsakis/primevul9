delete_keydata(dns_db_t *db, dns_dbversion_t *ver, dns_diff_t *diff,
	       dns_name_t *name, dns_rdataset_t *rdataset)
{
	dns_rdata_t rdata = DNS_RDATA_INIT;
	isc_result_t result, uresult;

	for (result = dns_rdataset_first(rdataset);
	     result == ISC_R_SUCCESS;
	     result = dns_rdataset_next(rdataset)) {
		dns_rdata_reset(&rdata);
		dns_rdataset_current(rdataset, &rdata);
		uresult = update_one_rr(db, ver, diff, DNS_DIFFOP_DEL,
					name, 0, &rdata);
		if (uresult != ISC_R_SUCCESS)
			return (uresult);
	}
	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;
	return (result);
}