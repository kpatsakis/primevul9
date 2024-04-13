isdn_timer_ctrl(int tf, int onoff)
{
	unsigned long flags;
	int old_tflags;

	spin_lock_irqsave(&dev->timerlock, flags);
	if ((tf & ISDN_TIMER_SLOW) && (!(dev->tflags & ISDN_TIMER_SLOW))) {
		/* If the slow-timer wasn't activated until now */
		isdn_timer_cnt1 = 0;
		isdn_timer_cnt2 = 0;
	}
	old_tflags = dev->tflags;
	if (onoff)
		dev->tflags |= tf;
	else
		dev->tflags &= ~tf;
	if (dev->tflags && !old_tflags)
		mod_timer(&dev->timer, jiffies + ISDN_TIMER_RES);
	spin_unlock_irqrestore(&dev->timerlock, flags);
}