static int update_timestamp_of_queue(struct snd_seq_event *event,
				     int queue, int real_time)
{
	struct snd_seq_queue *q;

	q = queueptr(queue);
	if (! q)
		return 0;
	event->queue = queue;
	event->flags &= ~SNDRV_SEQ_TIME_STAMP_MASK;
	if (real_time) {
		event->time.time = snd_seq_timer_get_cur_time(q->timer);
		event->flags |= SNDRV_SEQ_TIME_STAMP_REAL;
	} else {
		event->time.tick = snd_seq_timer_get_cur_tick(q->timer);
		event->flags |= SNDRV_SEQ_TIME_STAMP_TICK;
	}
	queuefree(q);
	return 1;
}