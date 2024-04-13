add_nsec(dns_db_t *db, dns_dbversion_t *version, dns_name_t *name,
	 dns_dbnode_t *node, dns_ttl_t ttl, bool bottom,
	 dns_diff_t *diff)
{
	dns_fixedname_t fixed;
	dns_name_t *next;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	isc_result_t result;
	unsigned char nsecbuffer[DNS_NSEC_BUFFERSIZE];

	next = dns_fixedname_initname(&fixed);

	CHECK(next_active(db, version, name, next, bottom));
	CHECK(dns_nsec_buildrdata(db, version, node, next, nsecbuffer,
				  &rdata));
	CHECK(update_one_rr(db, version, diff, DNS_DIFFOP_ADD, name, ttl,
			    &rdata));
 failure:
	return (result);
}