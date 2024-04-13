int mac802154_llsec_devkey_del(struct mac802154_llsec *sec,
			       __le64 dev_addr,
			       const struct ieee802154_llsec_device_key *key)
{
	struct mac802154_llsec_device *dev;
	struct mac802154_llsec_device_key *devkey;

	dev = llsec_dev_find_long(sec, dev_addr);

	if (!dev)
		return -ENOENT;

	devkey = llsec_devkey_find(dev, &key->key_id);
	if (!devkey)
		return -ENOENT;

	list_del_rcu(&devkey->devkey.list);
	kfree_rcu(devkey, rcu);
	return 0;
}