zone_timer(isc_task_t *task, isc_event_t *event) {
	const char me[] = "zone_timer";
	dns_zone_t *zone = (dns_zone_t *)event->ev_arg;

	UNUSED(task);
	REQUIRE(DNS_ZONE_VALID(zone));

	ENTER;

	zone_maintenance(zone);

	isc_event_free(&event);
}