int snd_ctl_remove(struct snd_card *card, struct snd_kcontrol *kcontrol)
{
	return __snd_ctl_remove(card, kcontrol, true);
}