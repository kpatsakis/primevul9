offline(dns_db_t *db, dns_dbversion_t *ver, dns__zonediff_t *zonediff,
	dns_name_t *name, dns_ttl_t ttl, dns_rdata_t *rdata)
{
	isc_result_t result;

	if ((rdata->flags & DNS_RDATA_OFFLINE) != 0)
		return (ISC_R_SUCCESS);
	result = update_one_rr(db, ver, zonediff->diff, DNS_DIFFOP_DELRESIGN,
			       name, ttl, rdata);
	if (result != ISC_R_SUCCESS)
		return (result);
	rdata->flags |= DNS_RDATA_OFFLINE;
	result = update_one_rr(db, ver, zonediff->diff, DNS_DIFFOP_ADDRESIGN,
			       name, ttl, rdata);
	zonediff->offline = true;
	return (result);
}