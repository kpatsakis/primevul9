ns_client_settimeout(ns_client_t *client, unsigned int seconds) {
	isc_result_t result;
	isc_interval_t interval;

	isc_interval_set(&interval, seconds, 0);
	result = isc_timer_reset(client->timer, isc_timertype_once, NULL,
				 &interval, false);
	client->timerset = true;
	if (result != ISC_R_SUCCESS) {
		ns_client_log(client, NS_LOGCATEGORY_CLIENT,
			      NS_LOGMODULE_CLIENT, ISC_LOG_ERROR,
			      "setting timeout: %s",
			      isc_result_totext(result));
		/* Continue anyway. */
	}
}