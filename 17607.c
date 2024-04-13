int snd_ctl_replace(struct snd_card *card, struct snd_kcontrol *kcontrol,
		    bool add_on_replace)
{
	return snd_ctl_add_replace(card, kcontrol,
				   add_on_replace ? CTL_ADD_ON_REPLACE : CTL_REPLACE);
}