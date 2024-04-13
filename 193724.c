int mac802154_llsec_key_del(struct mac802154_llsec *sec,
			    const struct ieee802154_llsec_key_id *key)
{
	struct ieee802154_llsec_key_entry *pos;

	list_for_each_entry(pos, &sec->table.keys, list) {
		struct mac802154_llsec_key *mkey;

		mkey = container_of(pos->key, struct mac802154_llsec_key, key);

		if (llsec_key_id_equal(&pos->id, key)) {
			list_del_rcu(&pos->list);
			llsec_key_put(mkey);
			return 0;
		}
	}

	return -ENOENT;
}