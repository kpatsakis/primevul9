int mac802154_llsec_devkey_add(struct mac802154_llsec *sec,
			       __le64 dev_addr,
			       const struct ieee802154_llsec_device_key *key)
{
	struct mac802154_llsec_device *dev;
	struct mac802154_llsec_device_key *devkey;

	dev = llsec_dev_find_long(sec, dev_addr);

	if (!dev)
		return -ENOENT;

	if (llsec_devkey_find(dev, &key->key_id))
		return -EEXIST;

	devkey = kmalloc(sizeof(*devkey), GFP_KERNEL);
	if (!devkey)
		return -ENOMEM;

	devkey->devkey = *key;
	list_add_tail_rcu(&devkey->devkey.list, &dev->dev.keys);
	return 0;
}