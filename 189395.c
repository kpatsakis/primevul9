static void snd_timer_process_callbacks(struct snd_timer *timer,
					struct list_head *head)
{
	struct snd_timer_instance *ti;
	unsigned long resolution, ticks;

	while (!list_empty(head)) {
		ti = list_first_entry(head, struct snd_timer_instance,
				      ack_list);

		/* remove from ack_list and make empty */
		list_del_init(&ti->ack_list);

		if (!(ti->flags & SNDRV_TIMER_IFLG_DEAD)) {
			ticks = ti->pticks;
			ti->pticks = 0;
			resolution = ti->resolution;
			ti->flags |= SNDRV_TIMER_IFLG_CALLBACK;
			spin_unlock(&timer->lock);
			if (ti->callback)
				ti->callback(ti, resolution, ticks);
			spin_lock(&timer->lock);
			ti->flags &= ~SNDRV_TIMER_IFLG_CALLBACK;
		}
	}
}