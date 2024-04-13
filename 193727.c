static void llsec_dev_free(struct mac802154_llsec_device *dev)
{
	struct ieee802154_llsec_device_key *pos, *pn;
	struct mac802154_llsec_device_key *devkey;

	list_for_each_entry_safe(pos, pn, &dev->dev.keys, list) {
		devkey = container_of(pos, struct mac802154_llsec_device_key,
				      devkey);

		list_del(&pos->list);
		kfree_sensitive(devkey);
	}

	kfree_sensitive(dev);
}