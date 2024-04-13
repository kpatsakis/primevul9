static int snd_ctl_open(struct inode *inode, struct file *file)
{
	unsigned long flags;
	struct snd_card *card;
	struct snd_ctl_file *ctl;
	int i, err;

	err = stream_open(inode, file);
	if (err < 0)
		return err;

	card = snd_lookup_minor_data(iminor(inode), SNDRV_DEVICE_TYPE_CONTROL);
	if (!card) {
		err = -ENODEV;
		goto __error1;
	}
	err = snd_card_file_add(card, file);
	if (err < 0) {
		err = -ENODEV;
		goto __error1;
	}
	if (!try_module_get(card->module)) {
		err = -EFAULT;
		goto __error2;
	}
	ctl = kzalloc(sizeof(*ctl), GFP_KERNEL);
	if (ctl == NULL) {
		err = -ENOMEM;
		goto __error;
	}
	INIT_LIST_HEAD(&ctl->events);
	init_waitqueue_head(&ctl->change_sleep);
	spin_lock_init(&ctl->read_lock);
	ctl->card = card;
	for (i = 0; i < SND_CTL_SUBDEV_ITEMS; i++)
		ctl->preferred_subdevice[i] = -1;
	ctl->pid = get_pid(task_pid(current));
	file->private_data = ctl;
	write_lock_irqsave(&card->ctl_files_rwlock, flags);
	list_add_tail(&ctl->list, &card->ctl_files);
	write_unlock_irqrestore(&card->ctl_files_rwlock, flags);
	snd_card_unref(card);
	return 0;

      __error:
	module_put(card->module);
      __error2:
	snd_card_file_remove(card, file);
      __error1:
	if (card)
		snd_card_unref(card);
      	return err;
}