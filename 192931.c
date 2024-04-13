static int snd_seq_ioctl_set_client_pool(struct snd_seq_client *client,
					 void *arg)
{
	struct snd_seq_client_pool *info = arg;
	int rc;

	if (client->number != info->client)
		return -EINVAL; /* can't change other clients */

	if (info->output_pool >= 1 && info->output_pool <= SNDRV_SEQ_MAX_EVENTS &&
	    (! snd_seq_write_pool_allocated(client) ||
	     info->output_pool != client->pool->size)) {
		if (snd_seq_write_pool_allocated(client)) {
			/* remove all existing cells */
			snd_seq_pool_mark_closing(client->pool);
			snd_seq_queue_client_leave_cells(client->number);
			snd_seq_pool_done(client->pool);
		}
		client->pool->size = info->output_pool;
		rc = snd_seq_pool_init(client->pool);
		if (rc < 0)
			return rc;
	}
	if (client->type == USER_CLIENT && client->data.user.fifo != NULL &&
	    info->input_pool >= 1 &&
	    info->input_pool <= SNDRV_SEQ_MAX_CLIENT_EVENTS &&
	    info->input_pool != client->data.user.fifo_pool_size) {
		/* change pool size */
		rc = snd_seq_fifo_resize(client->data.user.fifo, info->input_pool);
		if (rc < 0)
			return rc;
		client->data.user.fifo_pool_size = info->input_pool;
	}
	if (info->output_room >= 1 &&
	    info->output_room <= client->pool->size) {
		client->pool->room  = info->output_room;
	}

	return snd_seq_ioctl_get_client_pool(client, arg);
}