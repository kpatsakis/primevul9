ns_clientmgr_destroy(ns_clientmgr_t **managerp) {
	isc_result_t result;
	ns_clientmgr_t *manager;
	ns_client_t *client;
	bool need_destroy = false, unlock = false;

	REQUIRE(managerp != NULL);
	manager = *managerp;
	REQUIRE(VALID_MANAGER(manager));

	MTRACE("destroy");

	/*
	 * Check for success because we may already be task-exclusive
	 * at this point.  Only if we succeed at obtaining an exclusive
	 * lock now will we need to relinquish it later.
	 */
	result = isc_task_beginexclusive(ns_g_server->task);
	if (result == ISC_R_SUCCESS)
		unlock = true;

	manager->exiting = true;

	for (client = ISC_LIST_HEAD(manager->clients);
	     client != NULL;
	     client = ISC_LIST_NEXT(client, link))
		isc_task_shutdown(client->task);

	if (ISC_LIST_EMPTY(manager->clients))
		need_destroy = true;

	if (unlock)
		isc_task_endexclusive(ns_g_server->task);

	if (need_destroy)
		clientmgr_destroy(manager);

	*managerp = NULL;
}