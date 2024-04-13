static int snd_ctl_card_info(struct snd_card *card, struct snd_ctl_file * ctl,
			     unsigned int cmd, void __user *arg)
{
	struct snd_ctl_card_info *info;

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (! info)
		return -ENOMEM;
	down_read(&snd_ioctl_rwsem);
	info->card = card->number;
	strscpy(info->id, card->id, sizeof(info->id));
	strscpy(info->driver, card->driver, sizeof(info->driver));
	strscpy(info->name, card->shortname, sizeof(info->name));
	strscpy(info->longname, card->longname, sizeof(info->longname));
	strscpy(info->mixername, card->mixername, sizeof(info->mixername));
	strscpy(info->components, card->components, sizeof(info->components));
	up_read(&snd_ioctl_rwsem);
	if (copy_to_user(arg, info, sizeof(struct snd_ctl_card_info))) {
		kfree(info);
		return -EFAULT;
	}
	kfree(info);
	return 0;
}