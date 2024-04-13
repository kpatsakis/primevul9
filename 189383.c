static int snd_timer_user_next_device(struct snd_timer_id __user *_tid)
{
	struct snd_timer_id id;
	struct snd_timer *timer;
	struct list_head *p;

	if (copy_from_user(&id, _tid, sizeof(id)))
		return -EFAULT;
	mutex_lock(&register_mutex);
	if (id.dev_class < 0) {		/* first item */
		if (list_empty(&snd_timer_list))
			snd_timer_user_zero_id(&id);
		else {
			timer = list_entry(snd_timer_list.next,
					   struct snd_timer, device_list);
			snd_timer_user_copy_id(&id, timer);
		}
	} else {
		switch (id.dev_class) {
		case SNDRV_TIMER_CLASS_GLOBAL:
			id.device = id.device < 0 ? 0 : id.device + 1;
			list_for_each(p, &snd_timer_list) {
				timer = list_entry(p, struct snd_timer, device_list);
				if (timer->tmr_class > SNDRV_TIMER_CLASS_GLOBAL) {
					snd_timer_user_copy_id(&id, timer);
					break;
				}
				if (timer->tmr_device >= id.device) {
					snd_timer_user_copy_id(&id, timer);
					break;
				}
			}
			if (p == &snd_timer_list)
				snd_timer_user_zero_id(&id);
			break;
		case SNDRV_TIMER_CLASS_CARD:
		case SNDRV_TIMER_CLASS_PCM:
			if (id.card < 0) {
				id.card = 0;
			} else {
				if (id.device < 0) {
					id.device = 0;
				} else {
					if (id.subdevice < 0)
						id.subdevice = 0;
					else if (id.subdevice < INT_MAX)
						id.subdevice++;
				}
			}
			list_for_each(p, &snd_timer_list) {
				timer = list_entry(p, struct snd_timer, device_list);
				if (timer->tmr_class > id.dev_class) {
					snd_timer_user_copy_id(&id, timer);
					break;
				}
				if (timer->tmr_class < id.dev_class)
					continue;
				if (timer->card->number > id.card) {
					snd_timer_user_copy_id(&id, timer);
					break;
				}
				if (timer->card->number < id.card)
					continue;
				if (timer->tmr_device > id.device) {
					snd_timer_user_copy_id(&id, timer);
					break;
				}
				if (timer->tmr_device < id.device)
					continue;
				if (timer->tmr_subdevice > id.subdevice) {
					snd_timer_user_copy_id(&id, timer);
					break;
				}
				if (timer->tmr_subdevice < id.subdevice)
					continue;
				snd_timer_user_copy_id(&id, timer);
				break;
			}
			if (p == &snd_timer_list)
				snd_timer_user_zero_id(&id);
			break;
		default:
			snd_timer_user_zero_id(&id);
		}
	}
	mutex_unlock(&register_mutex);
	if (copy_to_user(_tid, &id, sizeof(*_tid)))
		return -EFAULT;
	return 0;
}