dns_zone_setisself(dns_zone_t *zone, dns_isselffunc_t isself, void *arg) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	zone->isself = isself;
	zone->isselfarg = arg;
	UNLOCK_ZONE(zone);
}