zone_journal(dns_zone_t *zone, dns_diff_t *diff, uint32_t *sourceserial,
	     const char *caller)
{
	const char me[] = "zone_journal";
	const char *journalfile;
	isc_result_t result = ISC_R_SUCCESS;
	dns_journal_t *journal = NULL;
	unsigned int mode = DNS_JOURNAL_CREATE|DNS_JOURNAL_WRITE;

	ENTER;
	journalfile = dns_zone_getjournal(zone);
	if (journalfile != NULL) {
		result = dns_journal_open(zone->mctx, journalfile, mode,
					  &journal);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "%s:dns_journal_open -> %s",
				     caller, dns_result_totext(result));
			return (result);
		}

		if (sourceserial != NULL)
			dns_journal_set_sourceserial(journal, *sourceserial);

		result = dns_journal_write_transaction(journal, diff);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "%s:dns_journal_write_transaction -> %s",
				     caller, dns_result_totext(result));
		}
		dns_journal_destroy(&journal);
	}

	return (result);
}