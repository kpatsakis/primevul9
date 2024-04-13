int snd_timer_close(struct snd_timer_instance *timeri)
{
	struct device *card_dev_to_put = NULL;
	int err;

	if (snd_BUG_ON(!timeri))
		return -ENXIO;

	mutex_lock(&register_mutex);
	err = snd_timer_close_locked(timeri, &card_dev_to_put);
	mutex_unlock(&register_mutex);
	/* put_device() is called after unlock for avoiding deadlock */
	if (card_dev_to_put)
		put_device(card_dev_to_put);
	return err;
}