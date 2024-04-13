ns_client_replace(ns_client_t *client) {
	isc_result_t result;
	bool tcp;

	CTRACE("replace");

	REQUIRE(client != NULL);
	REQUIRE(client->manager != NULL);

	tcp = TCP_CLIENT(client);
	if (tcp && client->pipelined) {
		result = get_worker(client->manager, client->interface,
				    client->tcpsocket);
	} else {
		result = get_client(client->manager, client->interface,
				    client->dispatch, tcp);
	}
	if (result != ISC_R_SUCCESS)
		return (result);

	/*
	 * The responsibility for listening for new requests is hereby
	 * transferred to the new client.  Therefore, the old client
	 * should refrain from listening for any more requests.
	 */
	client->mortal = true;

	return (ISC_R_SUCCESS);
}