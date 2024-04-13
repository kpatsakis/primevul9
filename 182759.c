zone_gotreadhandle(isc_task_t *task, isc_event_t *event) {
	dns_load_t *load = event->ev_arg;
	isc_result_t result = ISC_R_SUCCESS;
	unsigned int options;

	REQUIRE(DNS_LOAD_VALID(load));

	if ((event->ev_attributes & ISC_EVENTATTR_CANCELED) != 0)
		result = ISC_R_CANCELED;
	isc_event_free(&event);
	if (result == ISC_R_CANCELED)
		goto fail;

	options = get_master_options(load->zone);

	result = dns_master_loadfileinc(load->zone->masterfile,
					dns_db_origin(load->db),
					dns_db_origin(load->db),
					load->zone->rdclass, options, 0,
					&load->callbacks, task,
					zone_loaddone, load,
					&load->zone->lctx,
					zone_registerinclude,
					load->zone, load->zone->mctx,
					load->zone->masterformat,
					load->zone->maxttl);
	if (result != ISC_R_SUCCESS && result != DNS_R_CONTINUE &&
	    result != DNS_R_SEENINCLUDE)
		goto fail;
	return;

 fail:
	zone_loaddone(load, result);
}