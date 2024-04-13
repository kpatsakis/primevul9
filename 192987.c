static int snd_seq_ioctl_set_queue_info(struct snd_seq_client *client,
					void *arg)
{
	struct snd_seq_queue_info *info = arg;
	struct snd_seq_queue *q;

	if (info->owner != client->number)
		return -EINVAL;

	/* change owner/locked permission */
	if (snd_seq_queue_check_access(info->queue, client->number)) {
		if (snd_seq_queue_set_owner(info->queue, client->number, info->locked) < 0)
			return -EPERM;
		if (info->locked)
			snd_seq_queue_use(info->queue, client->number, 1);
	} else {
		return -EPERM;
	}	

	q = queueptr(info->queue);
	if (! q)
		return -EINVAL;
	if (q->owner != client->number) {
		queuefree(q);
		return -EPERM;
	}
	strlcpy(q->name, info->name, sizeof(q->name));
	queuefree(q);

	return 0;
}