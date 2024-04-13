void snd_seq_info_clients_read(struct snd_info_entry *entry, 
			       struct snd_info_buffer *buffer)
{
	int c;
	struct snd_seq_client *client;

	snd_iprintf(buffer, "Client info\n");
	snd_iprintf(buffer, "  cur  clients : %d\n", client_usage.cur);
	snd_iprintf(buffer, "  peak clients : %d\n", client_usage.peak);
	snd_iprintf(buffer, "  max  clients : %d\n", SNDRV_SEQ_MAX_CLIENTS);
	snd_iprintf(buffer, "\n");

	/* list the client table */
	for (c = 0; c < SNDRV_SEQ_MAX_CLIENTS; c++) {
		client = snd_seq_client_use_ptr(c);
		if (client == NULL)
			continue;
		if (client->type == NO_CLIENT) {
			snd_seq_client_unlock(client);
			continue;
		}

		snd_iprintf(buffer, "Client %3d : \"%s\" [%s]\n",
			    c, client->name,
			    client->type == USER_CLIENT ? "User" : "Kernel");
		snd_seq_info_dump_ports(buffer, client);
		if (snd_seq_write_pool_allocated(client)) {
			snd_iprintf(buffer, "  Output pool :\n");
			snd_seq_info_pool(buffer, client->pool, "    ");
		}
		if (client->type == USER_CLIENT && client->data.user.fifo &&
		    client->data.user.fifo->pool) {
			snd_iprintf(buffer, "  Input pool :\n");
			snd_seq_info_pool(buffer, client->data.user.fifo->pool, "    ");
		}
		snd_seq_client_unlock(client);
	}
}