dns_zone_setserialupdatemethod(dns_zone_t *zone, dns_updatemethod_t method) {
	REQUIRE(DNS_ZONE_VALID(zone));
	zone->updatemethod = method;
}