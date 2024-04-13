dns_zone_setjournalsize(dns_zone_t *zone, int32_t size) {
	REQUIRE(DNS_ZONE_VALID(zone));

	zone->journalsize = size;
}