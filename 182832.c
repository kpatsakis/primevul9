dns_zone_setnotifytype(dns_zone_t *zone, dns_notifytype_t notifytype) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	zone->notifytype = notifytype;
	UNLOCK_ZONE(zone);
}