client_shutdown(isc_task_t *task, isc_event_t *event) {
	ns_client_t *client;

	REQUIRE(event != NULL);
	REQUIRE(event->ev_type == ISC_TASKEVENT_SHUTDOWN);
	client = event->ev_arg;
	REQUIRE(NS_CLIENT_VALID(client));
	REQUIRE(task == client->task);

	UNUSED(task);

	CTRACE("shutdown");

	isc_event_free(&event);

	if (client->shutdown != NULL) {
		(client->shutdown)(client->shutdown_arg, ISC_R_SHUTTINGDOWN);
		client->shutdown = NULL;
		client->shutdown_arg = NULL;
	}

	if (ISC_QLINK_LINKED(client, ilink))
		ISC_QUEUE_UNLINK(client->manager->inactive, client, ilink);

	client->newstate = NS_CLIENTSTATE_FREED;
	client->needshutdown = false;
	(void)exit_check(client);
}