client_start(isc_task_t *task, isc_event_t *event) {
	ns_client_t *client = (ns_client_t *) event->ev_arg;

	INSIST(task == client->task);

	UNUSED(task);

	INSIST(client->nctls == 1);
	client->nctls--;

	if (exit_check(client))
		return;

	if (TCP_CLIENT(client)) {
		if (client->pipelined) {
			client_read(client);
		} else {
			client_accept(client);
		}
	} else {
		client_udprecv(client);
	}
}