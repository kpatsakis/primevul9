static void ml_effect_timer(struct timer_list *t)
{
	struct ml_device *ml = from_timer(ml, t, timer);
	struct input_dev *dev = ml->dev;
	unsigned long flags;

	pr_debug("timer: updating effects\n");

	spin_lock_irqsave(&dev->event_lock, flags);
	ml_play_effects(ml);
	spin_unlock_irqrestore(&dev->event_lock, flags);
}