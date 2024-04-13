int snd_ctl_get_preferred_subdevice(struct snd_card *card, int type)
{
	struct snd_ctl_file *kctl;
	int subdevice = -1;
	unsigned long flags;

	read_lock_irqsave(&card->ctl_files_rwlock, flags);
	list_for_each_entry(kctl, &card->ctl_files, list) {
		if (kctl->pid == task_pid(current)) {
			subdevice = kctl->preferred_subdevice[type];
			if (subdevice != -1)
				break;
		}
	}
	read_unlock_irqrestore(&card->ctl_files_rwlock, flags);
	return subdevice;
}