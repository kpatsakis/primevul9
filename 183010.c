dns_zone_gettask(dns_zone_t *zone, isc_task_t **target) {
	REQUIRE(DNS_ZONE_VALID(zone));
	isc_task_attach(zone->task, target);
}