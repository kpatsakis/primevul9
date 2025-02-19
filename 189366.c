static int snd_timer_check_master(struct snd_timer_instance *master)
{
	struct snd_timer_instance *slave, *tmp;

	/* check all pending slaves */
	list_for_each_entry_safe(slave, tmp, &snd_timer_slave_list, open_list) {
		if (slave->slave_class == master->slave_class &&
		    slave->slave_id == master->slave_id) {
			if (master->timer->num_instances >=
			    master->timer->max_instances)
				return -EBUSY;
			list_move_tail(&slave->open_list, &master->slave_list_head);
			master->timer->num_instances++;
			spin_lock_irq(&slave_active_lock);
			spin_lock(&master->timer->lock);
			slave->master = master;
			slave->timer = master->timer;
			if (slave->flags & SNDRV_TIMER_IFLG_RUNNING)
				list_add_tail(&slave->active_list,
					      &master->slave_active_head);
			spin_unlock(&master->timer->lock);
			spin_unlock_irq(&slave_active_lock);
		}
	}
	return 0;
}