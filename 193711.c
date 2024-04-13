llsec_update_devkey_record(struct mac802154_llsec_device *dev,
			   const struct ieee802154_llsec_key_id *in_key)
{
	struct mac802154_llsec_device_key *devkey;

	devkey = llsec_devkey_find(dev, in_key);

	if (!devkey) {
		struct mac802154_llsec_device_key *next;

		next = kzalloc(sizeof(*devkey), GFP_ATOMIC);
		if (!next)
			return -ENOMEM;

		next->devkey.key_id = *in_key;

		spin_lock_bh(&dev->lock);

		devkey = llsec_devkey_find(dev, in_key);
		if (!devkey)
			list_add_rcu(&next->devkey.list, &dev->dev.keys);
		else
			kfree_sensitive(next);

		spin_unlock_bh(&dev->lock);
	}

	return 0;
}