static int snd_timer_stop1(struct snd_timer_instance *timeri, bool stop)
{
	struct snd_timer *timer;
	int result = 0;
	unsigned long flags;

	timer = timeri->timer;
	if (!timer)
		return -EINVAL;
	spin_lock_irqsave(&timer->lock, flags);
	if (!(timeri->flags & (SNDRV_TIMER_IFLG_RUNNING |
			       SNDRV_TIMER_IFLG_START))) {
		result = -EBUSY;
		goto unlock;
	}
	list_del_init(&timeri->ack_list);
	list_del_init(&timeri->active_list);
	if (timer->card && timer->card->shutdown)
		goto unlock;
	if (stop) {
		timeri->cticks = timeri->ticks;
		timeri->pticks = 0;
	}
	if ((timeri->flags & SNDRV_TIMER_IFLG_RUNNING) &&
	    !(--timer->running)) {
		timer->hw.stop(timer);
		if (timer->flags & SNDRV_TIMER_FLG_RESCHED) {
			timer->flags &= ~SNDRV_TIMER_FLG_RESCHED;
			snd_timer_reschedule(timer, 0);
			if (timer->flags & SNDRV_TIMER_FLG_CHANGE) {
				timer->flags &= ~SNDRV_TIMER_FLG_CHANGE;
				timer->hw.start(timer);
			}
		}
	}
	timeri->flags &= ~(SNDRV_TIMER_IFLG_RUNNING | SNDRV_TIMER_IFLG_START);
	if (stop)
		timeri->flags &= ~SNDRV_TIMER_IFLG_PAUSED;
	else
		timeri->flags |= SNDRV_TIMER_IFLG_PAUSED;
	snd_timer_notify1(timeri, stop ? SNDRV_TIMER_EVENT_STOP :
			  SNDRV_TIMER_EVENT_PAUSE);
 unlock:
	spin_unlock_irqrestore(&timer->lock, flags);
	return result;
}