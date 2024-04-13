void serveconnection(CLIENT *client) {
	splitexport(client);

	if (!client->server->expected_size) {
		client->exportsize = size_autodetect(g_array_index(client->export,int,0));
	} else {
		/* Perhaps we should check first. Not now. */
		client->exportsize = client->server->expected_size;
	}
	if (client->exportsize > OFFT_MAX) {
		/* uhm, well... In a parallel universe, this *might* be
		 * possible... */
		err("Size of exported file is too big\n");
	}
	else {
		msg3(LOG_INFO, "size of exported file/device is %Lu", (unsigned long long)client->exportsize);
	}

	if (client->server->flags & F_COPYONWRITE) {
		copyonwrite_prepare(client);
	}

	setmysockopt(client->net);

	mainloop(client);
}