static int snd_seq_ioctl_get_named_queue(struct snd_seq_client *client,
					 void *arg)
{
	struct snd_seq_queue_info *info = arg;
	struct snd_seq_queue *q;

	q = snd_seq_queue_find_name(info->name);
	if (q == NULL)
		return -EINVAL;
	info->queue = q->queue;
	info->owner = q->owner;
	info->locked = q->locked;
	queuefree(q);

	return 0;
}