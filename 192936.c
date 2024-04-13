int snd_seq_create_kernel_client(struct snd_card *card, int client_index,
				 const char *name_fmt, ...)
{
	struct snd_seq_client *client;
	va_list args;

	if (snd_BUG_ON(in_interrupt()))
		return -EBUSY;

	if (card && client_index >= SNDRV_SEQ_CLIENTS_PER_CARD)
		return -EINVAL;
	if (card == NULL && client_index >= SNDRV_SEQ_GLOBAL_CLIENTS)
		return -EINVAL;

	if (mutex_lock_interruptible(&register_mutex))
		return -ERESTARTSYS;

	if (card) {
		client_index += SNDRV_SEQ_GLOBAL_CLIENTS
			+ card->number * SNDRV_SEQ_CLIENTS_PER_CARD;
		if (client_index >= SNDRV_SEQ_DYNAMIC_CLIENTS_BEGIN)
			client_index = -1;
	}

	/* empty write queue as default */
	client = seq_create_client1(client_index, 0);
	if (client == NULL) {
		mutex_unlock(&register_mutex);
		return -EBUSY;	/* failure code */
	}
	usage_alloc(&client_usage, 1);

	client->accept_input = 1;
	client->accept_output = 1;
	client->data.kernel.card = card;
		
	va_start(args, name_fmt);
	vsnprintf(client->name, sizeof(client->name), name_fmt, args);
	va_end(args);

	client->type = KERNEL_CLIENT;
	mutex_unlock(&register_mutex);

	/* make others aware this new client */
	snd_seq_system_client_ev_client_start(client->number);
	
	/* return client number to caller */
	return client->number;
}