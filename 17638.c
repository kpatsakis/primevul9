static int snd_ctl_add_replace(struct snd_card *card,
			       struct snd_kcontrol *kcontrol,
			       enum snd_ctl_add_mode mode)
{
	int err = -EINVAL;

	if (! kcontrol)
		return err;
	if (snd_BUG_ON(!card || !kcontrol->info))
		goto error;

	down_write(&card->controls_rwsem);
	err = __snd_ctl_add_replace(card, kcontrol, mode);
	up_write(&card->controls_rwsem);
	if (err < 0)
		goto error;
	return 0;

 error:
	snd_ctl_free_one(kcontrol);
	return err;
}