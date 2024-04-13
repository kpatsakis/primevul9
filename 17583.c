static int snd_ctl_dev_disconnect(struct snd_device *device)
{
	struct snd_card *card = device->device_data;
	struct snd_ctl_file *ctl;
	struct snd_ctl_layer_ops *lops;
	unsigned long flags;

	read_lock_irqsave(&card->ctl_files_rwlock, flags);
	list_for_each_entry(ctl, &card->ctl_files, list) {
		wake_up(&ctl->change_sleep);
		snd_kill_fasync(ctl->fasync, SIGIO, POLL_ERR);
	}
	read_unlock_irqrestore(&card->ctl_files_rwlock, flags);

	down_read(&card->controls_rwsem);
	down_read(&snd_ctl_layer_rwsem);
	for (lops = snd_ctl_layer; lops; lops = lops->next)
		lops->ldisconnect(card);
	up_read(&snd_ctl_layer_rwsem);
	up_read(&card->controls_rwsem);

	return snd_unregister_device(&card->ctl_dev);
}