process_adb_event(isc_task_t *task, isc_event_t *ev) {
	dns_notify_t *notify;
	isc_eventtype_t result;

	UNUSED(task);

	notify = ev->ev_arg;
	REQUIRE(DNS_NOTIFY_VALID(notify));
	INSIST(task == notify->zone->task);
	result = ev->ev_type;
	isc_event_free(&ev);
	if (result == DNS_EVENT_ADBMOREADDRESSES) {
		dns_adb_destroyfind(&notify->find);
		notify_find_address(notify);
		return;
	}
	if (result == DNS_EVENT_ADBNOMOREADDRESSES) {
		LOCK_ZONE(notify->zone);
		notify_send(notify);
		UNLOCK_ZONE(notify->zone);
	}
	notify_destroy(notify, false);
}