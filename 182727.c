dns_zone_setssutable(dns_zone_t *zone, dns_ssutable_t *table) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	if (zone->ssutable != NULL)
		dns_ssutable_detach(&zone->ssutable);
	if (table != NULL)
		dns_ssutable_attach(table, &zone->ssutable);
	UNLOCK_ZONE(zone);
}