static int snd_timer_start_slave(struct snd_timer_instance *timeri,
				 bool start)
{
	unsigned long flags;
	int err;

	spin_lock_irqsave(&slave_active_lock, flags);
	if (timeri->flags & SNDRV_TIMER_IFLG_DEAD) {
		err = -EINVAL;
		goto unlock;
	}
	if (timeri->flags & SNDRV_TIMER_IFLG_RUNNING) {
		err = -EBUSY;
		goto unlock;
	}
	timeri->flags |= SNDRV_TIMER_IFLG_RUNNING;
	if (timeri->master && timeri->timer) {
		spin_lock(&timeri->timer->lock);
		list_add_tail(&timeri->active_list,
			      &timeri->master->slave_active_head);
		snd_timer_notify1(timeri, start ? SNDRV_TIMER_EVENT_START :
				  SNDRV_TIMER_EVENT_CONTINUE);
		spin_unlock(&timeri->timer->lock);
	}
	err = 1; /* delayed start */
 unlock:
	spin_unlock_irqrestore(&slave_active_lock, flags);
	return err;
}