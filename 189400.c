static int snd_timer_check_slave(struct snd_timer_instance *slave)
{
	struct snd_timer *timer;
	struct snd_timer_instance *master;

	/* FIXME: it's really dumb to look up all entries.. */
	list_for_each_entry(timer, &snd_timer_list, device_list) {
		list_for_each_entry(master, &timer->open_list_head, open_list) {
			if (slave->slave_class == master->slave_class &&
			    slave->slave_id == master->slave_id) {
				if (master->timer->num_instances >=
				    master->timer->max_instances)
					return -EBUSY;
				list_move_tail(&slave->open_list,
					       &master->slave_list_head);
				master->timer->num_instances++;
				spin_lock_irq(&slave_active_lock);
				slave->master = master;
				slave->timer = master->timer;
				spin_unlock_irq(&slave_active_lock);
				return 0;
			}
		}
	}
	return 0;
}