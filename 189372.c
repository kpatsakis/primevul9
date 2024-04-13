int snd_timer_new(struct snd_card *card, char *id, struct snd_timer_id *tid,
		  struct snd_timer **rtimer)
{
	struct snd_timer *timer;
	int err;
	static struct snd_device_ops ops = {
		.dev_free = snd_timer_dev_free,
		.dev_register = snd_timer_dev_register,
		.dev_disconnect = snd_timer_dev_disconnect,
	};

	if (snd_BUG_ON(!tid))
		return -EINVAL;
	if (tid->dev_class == SNDRV_TIMER_CLASS_CARD ||
	    tid->dev_class == SNDRV_TIMER_CLASS_PCM) {
		if (WARN_ON(!card))
			return -EINVAL;
	}
	if (rtimer)
		*rtimer = NULL;
	timer = kzalloc(sizeof(*timer), GFP_KERNEL);
	if (!timer)
		return -ENOMEM;
	timer->tmr_class = tid->dev_class;
	timer->card = card;
	timer->tmr_device = tid->device;
	timer->tmr_subdevice = tid->subdevice;
	if (id)
		strlcpy(timer->id, id, sizeof(timer->id));
	timer->sticks = 1;
	INIT_LIST_HEAD(&timer->device_list);
	INIT_LIST_HEAD(&timer->open_list_head);
	INIT_LIST_HEAD(&timer->active_list_head);
	INIT_LIST_HEAD(&timer->ack_list_head);
	INIT_LIST_HEAD(&timer->sack_list_head);
	spin_lock_init(&timer->lock);
	tasklet_init(&timer->task_queue, snd_timer_tasklet,
		     (unsigned long)timer);
	timer->max_instances = 1000; /* default limit per timer */
	if (card != NULL) {
		timer->module = card->module;
		err = snd_device_new(card, SNDRV_DEV_TIMER, timer, &ops);
		if (err < 0) {
			snd_timer_free(timer);
			return err;
		}
	}
	if (rtimer)
		*rtimer = timer;
	return 0;
}