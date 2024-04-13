dns_zone_getrequeststats(dns_zone_t *zone) {
	/*
	 * We don't lock zone for efficiency reason.  This is not catastrophic
	 * because requeststats must always be valid when requeststats_on is
	 * true.
	 * Some counters may be incremented while requeststats_on is becoming
	 * false, or some cannot be incremented just after the statistics are
	 * installed, but it shouldn't matter much in practice.
	 */
	if (zone->requeststats_on)
		return (zone->requeststats);
	else
		return (NULL);
}