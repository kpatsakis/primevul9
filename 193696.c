llsec_devkey_find(struct mac802154_llsec_device *dev,
		  const struct ieee802154_llsec_key_id *key)
{
	struct ieee802154_llsec_device_key *devkey;

	list_for_each_entry_rcu(devkey, &dev->dev.keys, list) {
		if (!llsec_key_id_equal(key, &devkey->key_id))
			continue;

		return container_of(devkey, struct mac802154_llsec_device_key,
				    devkey);
	}

	return NULL;
}