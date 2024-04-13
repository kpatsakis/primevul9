dns_zone_getserialupdatemethod(dns_zone_t *zone) {
	REQUIRE(DNS_ZONE_VALID(zone));
	return(zone->updatemethod);
}