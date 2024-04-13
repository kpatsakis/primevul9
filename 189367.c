static int snd_timer_close_locked(struct snd_timer_instance *timeri,
				  struct device **card_devp_to_put)
{
	struct snd_timer *timer = timeri->timer;
	struct snd_timer_instance *slave, *tmp;

	if (timer) {
		spin_lock_irq(&timer->lock);
		timeri->flags |= SNDRV_TIMER_IFLG_DEAD;
		spin_unlock_irq(&timer->lock);
	}

	list_del(&timeri->open_list);

	/* force to stop the timer */
	snd_timer_stop(timeri);

	if (timer) {
		timer->num_instances--;
		/* wait, until the active callback is finished */
		spin_lock_irq(&timer->lock);
		while (timeri->flags & SNDRV_TIMER_IFLG_CALLBACK) {
			spin_unlock_irq(&timer->lock);
			udelay(10);
			spin_lock_irq(&timer->lock);
		}
		spin_unlock_irq(&timer->lock);

		/* remove slave links */
		spin_lock_irq(&slave_active_lock);
		spin_lock(&timer->lock);
		list_for_each_entry_safe(slave, tmp, &timeri->slave_list_head,
					 open_list) {
			list_move_tail(&slave->open_list, &snd_timer_slave_list);
			timer->num_instances--;
			slave->master = NULL;
			slave->timer = NULL;
			list_del_init(&slave->ack_list);
			list_del_init(&slave->active_list);
		}
		spin_unlock(&timer->lock);
		spin_unlock_irq(&slave_active_lock);

		/* slave doesn't need to release timer resources below */
		if (timeri->flags & SNDRV_TIMER_IFLG_SLAVE)
			timer = NULL;
	}

	if (timeri->private_free)
		timeri->private_free(timeri);
	kfree(timeri->owner);
	kfree(timeri);

	if (timer) {
		if (list_empty(&timer->open_list_head) && timer->hw.close)
			timer->hw.close(timer);
		/* release a card refcount for safe disconnection */
		if (timer->card)
			*card_devp_to_put = &timer->card->card_dev;
		module_put(timer->module);
	}

	return 0;
}