static int snd_ctl_release(struct inode *inode, struct file *file)
{
	unsigned long flags;
	struct snd_card *card;
	struct snd_ctl_file *ctl;
	struct snd_kcontrol *control;
	unsigned int idx;

	ctl = file->private_data;
	file->private_data = NULL;
	card = ctl->card;
	write_lock_irqsave(&card->ctl_files_rwlock, flags);
	list_del(&ctl->list);
	write_unlock_irqrestore(&card->ctl_files_rwlock, flags);
	down_write(&card->controls_rwsem);
	list_for_each_entry(control, &card->controls, list)
		for (idx = 0; idx < control->count; idx++)
			if (control->vd[idx].owner == ctl)
				control->vd[idx].owner = NULL;
	up_write(&card->controls_rwsem);
	snd_fasync_free(ctl->fasync);
	snd_ctl_empty_read_queue(ctl);
	put_pid(ctl->pid);
	kfree(ctl);
	module_put(card->module);
	snd_card_file_remove(card, file);
	return 0;
}