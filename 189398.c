static int snd_timer_stop_slave(struct snd_timer_instance *timeri, bool stop)
{
	unsigned long flags;

	spin_lock_irqsave(&slave_active_lock, flags);
	if (!(timeri->flags & SNDRV_TIMER_IFLG_RUNNING)) {
		spin_unlock_irqrestore(&slave_active_lock, flags);
		return -EBUSY;
	}
	timeri->flags &= ~SNDRV_TIMER_IFLG_RUNNING;
	if (timeri->timer) {
		spin_lock(&timeri->timer->lock);
		list_del_init(&timeri->ack_list);
		list_del_init(&timeri->active_list);
		snd_timer_notify1(timeri, stop ? SNDRV_TIMER_EVENT_STOP :
				  SNDRV_TIMER_EVENT_PAUSE);
		spin_unlock(&timeri->timer->lock);
	}
	spin_unlock_irqrestore(&slave_active_lock, flags);
	return 0;
}