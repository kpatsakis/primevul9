dns_zone_getjournalsize(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));

	return (zone->journalsize);
}