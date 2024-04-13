static void add_hash_entries(struct snd_card *card,
			     struct snd_kcontrol *kcontrol)
{
	struct snd_ctl_elem_id id = kcontrol->id;
	int i;

	xa_store_range(&card->ctl_numids, kcontrol->id.numid,
		       kcontrol->id.numid + kcontrol->count - 1,
		       kcontrol, GFP_KERNEL);

	for (i = 0; i < kcontrol->count; i++) {
		id.index = kcontrol->id.index + i;
		if (xa_insert(&card->ctl_hash, get_ctl_id_hash(&id),
			      kcontrol, GFP_KERNEL)) {
			/* skip hash for this entry, noting we had collision */
			card->ctl_hash_collision = true;
			dev_dbg(card->dev, "ctl_hash collision %d:%s:%d\n",
				id.iface, id.name, id.index);
		}
	}
}