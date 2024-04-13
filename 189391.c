static void snd_timer_tasklet(unsigned long arg)
{
	struct snd_timer *timer = (struct snd_timer *) arg;
	unsigned long flags;

	if (timer->card && timer->card->shutdown) {
		snd_timer_clear_callbacks(timer, &timer->sack_list_head);
		return;
	}

	spin_lock_irqsave(&timer->lock, flags);
	snd_timer_process_callbacks(timer, &timer->sack_list_head);
	spin_unlock_irqrestore(&timer->lock, flags);
}