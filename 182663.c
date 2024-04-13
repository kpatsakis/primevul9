sync_secure_journal(dns_zone_t *zone, dns_zone_t *raw, dns_journal_t *journal,
		    uint32_t start, uint32_t end,
		    dns_difftuple_t **soatuplep, dns_diff_t *diff)
{
	isc_result_t result;
	dns_difftuple_t *tuple = NULL;
	dns_diffop_t op = DNS_DIFFOP_ADD;
	int n_soa = 0;

	REQUIRE(soatuplep != NULL);

	if (start == end)
		return (DNS_R_UNCHANGED);

	CHECK(dns_journal_iter_init(journal, start, end));
	for (result = dns_journal_first_rr(journal);
	     result == ISC_R_SUCCESS;
	     result = dns_journal_next_rr(journal))
	{
		dns_name_t *name = NULL;
		uint32_t ttl;
		dns_rdata_t *rdata = NULL;
		dns_journal_current_rr(journal, &name, &ttl, &rdata);

		if (rdata->type == dns_rdatatype_soa) {
			n_soa++;
			if (n_soa == 2) {
				/*
				 * Save the latest raw SOA record.
				 */
				if (*soatuplep != NULL)
					dns_difftuple_free(soatuplep);
				CHECK(dns_difftuple_create(diff->mctx,
							   DNS_DIFFOP_ADD,
							   name, ttl, rdata,
							   soatuplep));
			}
			if (n_soa == 3)
				n_soa = 1;
			continue;
		}

		/* Sanity. */
		if (n_soa == 0) {
			dns_zone_log(raw, ISC_LOG_ERROR,
				     "corrupt journal file: '%s'\n",
				     raw->journal);
			return (ISC_R_FAILURE);
		}

		if (zone->privatetype != 0 &&
		    rdata->type == zone->privatetype)
			continue;

		if (rdata->type == dns_rdatatype_nsec ||
		    rdata->type == dns_rdatatype_rrsig ||
		    rdata->type == dns_rdatatype_nsec3 ||
		    rdata->type == dns_rdatatype_dnskey ||
		    rdata->type == dns_rdatatype_nsec3param)
			continue;

		op = (n_soa == 1) ? DNS_DIFFOP_DEL : DNS_DIFFOP_ADD;

		CHECK(dns_difftuple_create(diff->mctx, op, name, ttl, rdata,
					   &tuple));
		dns_diff_appendminimal(diff, &tuple);
	}
	if (result == ISC_R_NOMORE)
		result = ISC_R_SUCCESS;

 failure:
	return(result);
}