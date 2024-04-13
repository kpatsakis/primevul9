static int snd_seq_ioctl_get_queue_status(struct snd_seq_client *client,
					  void *arg)
{
	struct snd_seq_queue_status *status = arg;
	struct snd_seq_queue *queue;
	struct snd_seq_timer *tmr;

	queue = queueptr(status->queue);
	if (queue == NULL)
		return -EINVAL;
	memset(status, 0, sizeof(*status));
	status->queue = queue->queue;
	
	tmr = queue->timer;
	status->events = queue->tickq->cells + queue->timeq->cells;

	status->time = snd_seq_timer_get_cur_time(tmr);
	status->tick = snd_seq_timer_get_cur_tick(tmr);

	status->running = tmr->running;

	status->flags = queue->flags;
	queuefree(queue);

	return 0;
}