int snd_ctl_rename_id(struct snd_card *card, struct snd_ctl_elem_id *src_id,
		      struct snd_ctl_elem_id *dst_id)
{
	struct snd_kcontrol *kctl;

	down_write(&card->controls_rwsem);
	kctl = snd_ctl_find_id(card, src_id);
	if (kctl == NULL) {
		up_write(&card->controls_rwsem);
		return -ENOENT;
	}
	remove_hash_entries(card, kctl);
	kctl->id = *dst_id;
	kctl->id.numid = card->last_numid + 1;
	card->last_numid += kctl->count;
	add_hash_entries(card, kctl);
	up_write(&card->controls_rwsem);
	return 0;
}