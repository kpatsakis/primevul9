void snd_timer_interrupt(struct snd_timer * timer, unsigned long ticks_left)
{
	struct snd_timer_instance *ti, *ts, *tmp;
	unsigned long resolution;
	struct list_head *ack_list_head;
	unsigned long flags;
	int use_tasklet = 0;

	if (timer == NULL)
		return;

	if (timer->card && timer->card->shutdown) {
		snd_timer_clear_callbacks(timer, &timer->ack_list_head);
		return;
	}

	spin_lock_irqsave(&timer->lock, flags);

	/* remember the current resolution */
	resolution = snd_timer_hw_resolution(timer);

	/* loop for all active instances
	 * Here we cannot use list_for_each_entry because the active_list of a
	 * processed instance is relinked to done_list_head before the callback
	 * is called.
	 */
	list_for_each_entry_safe(ti, tmp, &timer->active_list_head,
				 active_list) {
		if (ti->flags & SNDRV_TIMER_IFLG_DEAD)
			continue;
		if (!(ti->flags & SNDRV_TIMER_IFLG_RUNNING))
			continue;
		ti->pticks += ticks_left;
		ti->resolution = resolution;
		if (ti->cticks < ticks_left)
			ti->cticks = 0;
		else
			ti->cticks -= ticks_left;
		if (ti->cticks) /* not expired */
			continue;
		if (ti->flags & SNDRV_TIMER_IFLG_AUTO) {
			ti->cticks = ti->ticks;
		} else {
			ti->flags &= ~SNDRV_TIMER_IFLG_RUNNING;
			--timer->running;
			list_del_init(&ti->active_list);
		}
		if ((timer->hw.flags & SNDRV_TIMER_HW_TASKLET) ||
		    (ti->flags & SNDRV_TIMER_IFLG_FAST))
			ack_list_head = &timer->ack_list_head;
		else
			ack_list_head = &timer->sack_list_head;
		if (list_empty(&ti->ack_list))
			list_add_tail(&ti->ack_list, ack_list_head);
		list_for_each_entry(ts, &ti->slave_active_head, active_list) {
			ts->pticks = ti->pticks;
			ts->resolution = resolution;
			if (list_empty(&ts->ack_list))
				list_add_tail(&ts->ack_list, ack_list_head);
		}
	}
	if (timer->flags & SNDRV_TIMER_FLG_RESCHED)
		snd_timer_reschedule(timer, timer->sticks);
	if (timer->running) {
		if (timer->hw.flags & SNDRV_TIMER_HW_STOP) {
			timer->hw.stop(timer);
			timer->flags |= SNDRV_TIMER_FLG_CHANGE;
		}
		if (!(timer->hw.flags & SNDRV_TIMER_HW_AUTO) ||
		    (timer->flags & SNDRV_TIMER_FLG_CHANGE)) {
			/* restart timer */
			timer->flags &= ~SNDRV_TIMER_FLG_CHANGE;
			timer->hw.start(timer);
		}
	} else {
		timer->hw.stop(timer);
	}

	/* now process all fast callbacks */
	snd_timer_process_callbacks(timer, &timer->ack_list_head);

	/* do we have any slow callbacks? */
	use_tasklet = !list_empty(&timer->sack_list_head);
	spin_unlock_irqrestore(&timer->lock, flags);

	if (use_tasklet)
		tasklet_schedule(&timer->task_queue);
}