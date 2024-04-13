static void remove_hash_entries(struct snd_card *card,
				struct snd_kcontrol *kcontrol)
{
	struct snd_ctl_elem_id id = kcontrol->id;
	struct snd_kcontrol *matched;
	unsigned long h;
	int i;

	for (i = 0; i < kcontrol->count; i++) {
		xa_erase(&card->ctl_numids, id.numid);
		h = get_ctl_id_hash(&id);
		matched = xa_load(&card->ctl_hash, h);
		if (matched && (matched == kcontrol ||
				elem_id_matches(matched, &id)))
			xa_erase(&card->ctl_hash, h);
		id.index++;
		id.numid++;
	}
}