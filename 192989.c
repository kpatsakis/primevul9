static int snd_seq_ioctl_create_queue(struct snd_seq_client *client, void *arg)
{
	struct snd_seq_queue_info *info = arg;
	struct snd_seq_queue *q;

	q = snd_seq_queue_alloc(client->number, info->locked, info->flags);
	if (IS_ERR(q))
		return PTR_ERR(q);

	info->queue = q->queue;
	info->locked = q->locked;
	info->owner = q->owner;

	/* set queue name */
	if (!info->name[0])
		snprintf(info->name, sizeof(info->name), "Queue-%d", q->queue);
	strlcpy(q->name, info->name, sizeof(q->name));
	snd_use_lock_free(&q->use_lock);

	return 0;
}