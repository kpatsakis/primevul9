static int snd_seq_ioctl_get_queue_info(struct snd_seq_client *client,
					void *arg)
{
	struct snd_seq_queue_info *info = arg;
	struct snd_seq_queue *q;

	q = queueptr(info->queue);
	if (q == NULL)
		return -EINVAL;

	memset(info, 0, sizeof(*info));
	info->queue = q->queue;
	info->owner = q->owner;
	info->locked = q->locked;
	strlcpy(info->name, q->name, sizeof(info->name));
	queuefree(q);

	return 0;
}