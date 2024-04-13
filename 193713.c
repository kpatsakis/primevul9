void mac802154_llsec_destroy(struct mac802154_llsec *sec)
{
	struct ieee802154_llsec_seclevel *sl, *sn;
	struct ieee802154_llsec_device *dev, *dn;
	struct ieee802154_llsec_key_entry *key, *kn;

	list_for_each_entry_safe(sl, sn, &sec->table.security_levels, list) {
		struct mac802154_llsec_seclevel *msl;

		msl = container_of(sl, struct mac802154_llsec_seclevel, level);
		list_del(&sl->list);
		kfree_sensitive(msl);
	}

	list_for_each_entry_safe(dev, dn, &sec->table.devices, list) {
		struct mac802154_llsec_device *mdev;

		mdev = container_of(dev, struct mac802154_llsec_device, dev);
		list_del(&dev->list);
		llsec_dev_free(mdev);
	}

	list_for_each_entry_safe(key, kn, &sec->table.keys, list) {
		struct mac802154_llsec_key *mkey;

		mkey = container_of(key->key, struct mac802154_llsec_key, key);
		list_del(&key->list);
		llsec_key_put(mkey);
		kfree_sensitive(key);
	}
}