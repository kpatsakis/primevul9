dns_zone_setxfrsource4dscp(dns_zone_t *zone, isc_dscp_t dscp) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	zone->xfrsource4dscp = dscp;
	UNLOCK_ZONE(zone);

	return (ISC_R_SUCCESS);
}