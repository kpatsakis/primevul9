dns_zone_catz_enable(dns_zone_t *zone, dns_catz_zones_t *catzs) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(catzs != NULL);

	LOCK_ZONE(zone);
	INSIST(zone->catzs == NULL || zone->catzs == catzs);
	dns_catz_catzs_set_view(catzs, zone->view);
	if (zone->catzs == NULL)
		dns_catz_catzs_attach(catzs, &zone->catzs);
	UNLOCK_ZONE(zone);
}