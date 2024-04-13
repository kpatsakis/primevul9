static int realloc_user_queue(struct snd_timer_user *tu, int size)
{
	struct snd_timer_read *queue = NULL;
	struct snd_timer_tread *tqueue = NULL;

	if (tu->tread) {
		tqueue = kcalloc(size, sizeof(*tqueue), GFP_KERNEL);
		if (!tqueue)
			return -ENOMEM;
	} else {
		queue = kcalloc(size, sizeof(*queue), GFP_KERNEL);
		if (!queue)
			return -ENOMEM;
	}

	spin_lock_irq(&tu->qlock);
	kfree(tu->queue);
	kfree(tu->tqueue);
	tu->queue_size = size;
	tu->queue = queue;
	tu->tqueue = tqueue;
	tu->qhead = tu->qtail = tu->qused = 0;
	spin_unlock_irq(&tu->qlock);

	return 0;
}