static int snd_seq_ioctl_set_queue_timer(struct snd_seq_client *client,
					 void *arg)
{
	struct snd_seq_queue_timer *timer = arg;
	int result = 0;

	if (timer->type != SNDRV_SEQ_TIMER_ALSA)
		return -EINVAL;

	if (snd_seq_queue_check_access(timer->queue, client->number)) {
		struct snd_seq_queue *q;
		struct snd_seq_timer *tmr;

		q = queueptr(timer->queue);
		if (q == NULL)
			return -ENXIO;
		if (mutex_lock_interruptible(&q->timer_mutex)) {
			queuefree(q);
			return -ERESTARTSYS;
		}
		tmr = q->timer;
		snd_seq_queue_timer_close(timer->queue);
		tmr->type = timer->type;
		if (tmr->type == SNDRV_SEQ_TIMER_ALSA) {
			tmr->alsa_id = timer->u.alsa.id;
			tmr->preferred_resolution = timer->u.alsa.resolution;
		}
		result = snd_seq_queue_timer_open(timer->queue);
		mutex_unlock(&q->timer_mutex);
		queuefree(q);
	} else {
		return -EPERM;
	}	

	return result;
}