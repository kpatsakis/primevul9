static void snd_ctl_empty_read_queue(struct snd_ctl_file * ctl)
{
	unsigned long flags;
	struct snd_kctl_event *cread;

	spin_lock_irqsave(&ctl->read_lock, flags);
	while (!list_empty(&ctl->events)) {
		cread = snd_kctl_event(ctl->events.next);
		list_del(&cread->list);
		kfree(cread);
	}
	spin_unlock_irqrestore(&ctl->read_lock, flags);
}