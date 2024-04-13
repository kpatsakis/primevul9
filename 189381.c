static void snd_timer_clear_callbacks(struct snd_timer *timer,
				      struct list_head *head)
{
	unsigned long flags;

	spin_lock_irqsave(&timer->lock, flags);
	while (!list_empty(head))
		list_del_init(head->next);
	spin_unlock_irqrestore(&timer->lock, flags);
}