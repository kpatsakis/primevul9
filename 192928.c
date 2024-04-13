static int seq_free_client1(struct snd_seq_client *client)
{
	unsigned long flags;

	if (!client)
		return 0;
	snd_seq_delete_all_ports(client);
	snd_seq_queue_client_leave(client->number);
	spin_lock_irqsave(&clients_lock, flags);
	clienttablock[client->number] = 1;
	clienttab[client->number] = NULL;
	spin_unlock_irqrestore(&clients_lock, flags);
	snd_use_lock_sync(&client->use_lock);
	snd_seq_queue_client_termination(client->number);
	if (client->pool)
		snd_seq_pool_delete(&client->pool);
	spin_lock_irqsave(&clients_lock, flags);
	clienttablock[client->number] = 0;
	spin_unlock_irqrestore(&clients_lock, flags);
	return 0;
}