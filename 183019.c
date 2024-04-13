dns_zone_setnotifysrc6(dns_zone_t *zone, const isc_sockaddr_t *notifysrc) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	zone->notifysrc6 = *notifysrc;
	UNLOCK_ZONE(zone);

	return (ISC_R_SUCCESS);
}