static int snd_ctl_dev_register(struct snd_device *device)
{
	struct snd_card *card = device->device_data;
	struct snd_ctl_layer_ops *lops;
	int err;

	err = snd_register_device(SNDRV_DEVICE_TYPE_CONTROL, card, -1,
				  &snd_ctl_f_ops, card, &card->ctl_dev);
	if (err < 0)
		return err;
	down_read(&card->controls_rwsem);
	down_read(&snd_ctl_layer_rwsem);
	for (lops = snd_ctl_layer; lops; lops = lops->next)
		lops->lregister(card);
	up_read(&snd_ctl_layer_rwsem);
	up_read(&card->controls_rwsem);
	return 0;
}