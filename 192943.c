struct snd_seq_client *snd_seq_client_use_ptr(int clientid)
{
	unsigned long flags;
	struct snd_seq_client *client;

	if (clientid < 0 || clientid >= SNDRV_SEQ_MAX_CLIENTS) {
		pr_debug("ALSA: seq: oops. Trying to get pointer to client %d\n",
			   clientid);
		return NULL;
	}
	spin_lock_irqsave(&clients_lock, flags);
	client = clientptr(clientid);
	if (client)
		goto __lock;
	if (clienttablock[clientid]) {
		spin_unlock_irqrestore(&clients_lock, flags);
		return NULL;
	}
	spin_unlock_irqrestore(&clients_lock, flags);
#ifdef CONFIG_MODULES
	if (!in_interrupt()) {
		static char client_requested[SNDRV_SEQ_GLOBAL_CLIENTS];
		static char card_requested[SNDRV_CARDS];
		if (clientid < SNDRV_SEQ_GLOBAL_CLIENTS) {
			int idx;
			
			if (!client_requested[clientid]) {
				client_requested[clientid] = 1;
				for (idx = 0; idx < 15; idx++) {
					if (seq_client_load[idx] < 0)
						break;
					if (seq_client_load[idx] == clientid) {
						request_module("snd-seq-client-%i",
							       clientid);
						break;
					}
				}
			}
		} else if (clientid < SNDRV_SEQ_DYNAMIC_CLIENTS_BEGIN) {
			int card = (clientid - SNDRV_SEQ_GLOBAL_CLIENTS) /
				SNDRV_SEQ_CLIENTS_PER_CARD;
			if (card < snd_ecards_limit) {
				if (! card_requested[card]) {
					card_requested[card] = 1;
					snd_request_card(card);
				}
				snd_seq_device_load_drivers();
			}
		}
		spin_lock_irqsave(&clients_lock, flags);
		client = clientptr(clientid);
		if (client)
			goto __lock;
		spin_unlock_irqrestore(&clients_lock, flags);
	}
#endif
	return NULL;

      __lock:
	snd_use_lock_use(&client->use_lock);
	spin_unlock_irqrestore(&clients_lock, flags);
	return client;
}