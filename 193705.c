int mac802154_llsec_key_add(struct mac802154_llsec *sec,
			    const struct ieee802154_llsec_key_id *id,
			    const struct ieee802154_llsec_key *key)
{
	struct mac802154_llsec_key *mkey = NULL;
	struct ieee802154_llsec_key_entry *pos, *new;

	if (!(key->frame_types & (1 << IEEE802154_FC_TYPE_MAC_CMD)) &&
	    key->cmd_frame_ids)
		return -EINVAL;

	list_for_each_entry(pos, &sec->table.keys, list) {
		if (llsec_key_id_equal(&pos->id, id))
			return -EEXIST;

		if (memcmp(pos->key->key, key->key,
			   IEEE802154_LLSEC_KEY_SIZE))
			continue;

		mkey = container_of(pos->key, struct mac802154_llsec_key, key);

		/* Don't allow multiple instances of the same AES key to have
		 * different allowed frame types/command frame ids, as this is
		 * not possible in the 802.15.4 PIB.
		 */
		if (pos->key->frame_types != key->frame_types ||
		    pos->key->cmd_frame_ids != key->cmd_frame_ids)
			return -EEXIST;

		break;
	}

	new = kzalloc(sizeof(*new), GFP_KERNEL);
	if (!new)
		return -ENOMEM;

	if (!mkey)
		mkey = llsec_key_alloc(key);
	else
		mkey = llsec_key_get(mkey);

	if (!mkey)
		goto fail;

	new->id = *id;
	new->key = &mkey->key;

	list_add_rcu(&new->list, &sec->table.keys);

	return 0;

fail:
	kfree_sensitive(new);
	return -ENOMEM;
}