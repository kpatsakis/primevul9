dns_zone_getraw(dns_zone_t *zone, dns_zone_t **raw) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(raw != NULL && *raw == NULL);

	LOCK(&zone->lock);
	INSIST(zone != zone->raw);
	if (zone->raw != NULL)
		dns_zone_attach(zone->raw, raw);
	UNLOCK(&zone->lock);
}