static int snd_ctl_dev_free(struct snd_device *device)
{
	struct snd_card *card = device->device_data;
	struct snd_kcontrol *control;

	down_write(&card->controls_rwsem);
	while (!list_empty(&card->controls)) {
		control = snd_kcontrol(card->controls.next);
		__snd_ctl_remove(card, control, false);
	}

#ifdef CONFIG_SND_CTL_FAST_LOOKUP
	xa_destroy(&card->ctl_numids);
	xa_destroy(&card->ctl_hash);
#endif
	up_write(&card->controls_rwsem);
	put_device(&card->ctl_dev);
	return 0;
}