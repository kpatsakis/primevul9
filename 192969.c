static __poll_t snd_seq_poll(struct file *file, poll_table * wait)
{
	struct snd_seq_client *client = file->private_data;
	__poll_t mask = 0;

	/* check client structures are in place */
	if (snd_BUG_ON(!client))
		return -ENXIO;

	if ((snd_seq_file_flags(file) & SNDRV_SEQ_LFLG_INPUT) &&
	    client->data.user.fifo) {

		/* check if data is available in the outqueue */
		if (snd_seq_fifo_poll_wait(client->data.user.fifo, file, wait))
			mask |= EPOLLIN | EPOLLRDNORM;
	}

	if (snd_seq_file_flags(file) & SNDRV_SEQ_LFLG_OUTPUT) {

		/* check if data is available in the pool */
		if (!snd_seq_write_pool_allocated(client) ||
		    snd_seq_pool_poll_wait(client->pool, file, wait))
			mask |= EPOLLOUT | EPOLLWRNORM;
	}

	return mask;
}