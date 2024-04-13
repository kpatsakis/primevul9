snd_ctl_find_numid_slow(struct snd_card *card, unsigned int numid)
{
	struct snd_kcontrol *kctl;

	list_for_each_entry(kctl, &card->controls, list) {
		if (kctl->id.numid <= numid && kctl->id.numid + kctl->count > numid)
			return kctl;
	}
	return NULL;
}