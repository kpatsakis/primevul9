unsigned long snd_timer_resolution(struct snd_timer_instance *timeri)
{
	struct snd_timer * timer;
	unsigned long ret = 0;
	unsigned long flags;

	if (timeri == NULL)
		return 0;
	timer = timeri->timer;
	if (timer) {
		spin_lock_irqsave(&timer->lock, flags);
		ret = snd_timer_hw_resolution(timer);
		spin_unlock_irqrestore(&timer->lock, flags);
	}
	return ret;
}