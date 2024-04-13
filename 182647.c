dns_zone_iattach(dns_zone_t *source, dns_zone_t **target) {
	REQUIRE(DNS_ZONE_VALID(source));
	REQUIRE(target != NULL && *target == NULL);
	LOCK_ZONE(source);
	zone_iattach(source, target);
	UNLOCK_ZONE(source);
}