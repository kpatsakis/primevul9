int snd_ctl_remove_id(struct snd_card *card, struct snd_ctl_elem_id *id)
{
	struct snd_kcontrol *kctl;
	int ret;

	down_write(&card->controls_rwsem);
	kctl = snd_ctl_find_id(card, id);
	if (kctl == NULL) {
		up_write(&card->controls_rwsem);
		return -ENOENT;
	}
	ret = snd_ctl_remove(card, kctl);
	up_write(&card->controls_rwsem);
	return ret;
}