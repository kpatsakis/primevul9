static void seq_free_client(struct snd_seq_client * client)
{
	mutex_lock(&register_mutex);
	switch (client->type) {
	case NO_CLIENT:
		pr_warn("ALSA: seq: Trying to free unused client %d\n",
			client->number);
		break;
	case USER_CLIENT:
	case KERNEL_CLIENT:
		seq_free_client1(client);
		usage_free(&client_usage, 1);
		break;

	default:
		pr_err("ALSA: seq: Trying to free client %d with undefined type = %d\n",
			   client->number, client->type);
	}
	mutex_unlock(&register_mutex);

	snd_seq_system_client_ev_client_exit(client->number);
}