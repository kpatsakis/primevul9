static int __snd_ctl_remove(struct snd_card *card,
			    struct snd_kcontrol *kcontrol,
			    bool remove_hash)
{
	unsigned int idx;

	if (snd_BUG_ON(!card || !kcontrol))
		return -EINVAL;
	list_del(&kcontrol->list);

	if (remove_hash)
		remove_hash_entries(card, kcontrol);

	card->controls_count -= kcontrol->count;
	for (idx = 0; idx < kcontrol->count; idx++)
		snd_ctl_notify_one(card, SNDRV_CTL_EVENT_MASK_REMOVE, kcontrol, idx);
	snd_ctl_free_one(kcontrol);
	return 0;
}