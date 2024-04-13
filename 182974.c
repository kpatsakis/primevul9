dns_zone_setcheckmx(dns_zone_t *zone, dns_checkmxfunc_t checkmx) {
	REQUIRE(DNS_ZONE_VALID(zone));
	zone->checkmx = checkmx;
}