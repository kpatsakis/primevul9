static int snd_seq_ioctl_get_queue_timer(struct snd_seq_client *client,
					 void *arg)
{
	struct snd_seq_queue_timer *timer = arg;
	struct snd_seq_queue *queue;
	struct snd_seq_timer *tmr;

	queue = queueptr(timer->queue);
	if (queue == NULL)
		return -EINVAL;

	if (mutex_lock_interruptible(&queue->timer_mutex)) {
		queuefree(queue);
		return -ERESTARTSYS;
	}
	tmr = queue->timer;
	memset(timer, 0, sizeof(*timer));
	timer->queue = queue->queue;

	timer->type = tmr->type;
	if (tmr->type == SNDRV_SEQ_TIMER_ALSA) {
		timer->u.alsa.id = tmr->alsa_id;
		timer->u.alsa.resolution = tmr->preferred_resolution;
	}
	mutex_unlock(&queue->timer_mutex);
	queuefree(queue);
	
	return 0;
}