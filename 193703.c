llsec_dev_find_long(struct mac802154_llsec *sec, __le64 hwaddr)
{
	struct mac802154_llsec_device *dev;
	u64 key = llsec_dev_hash_long(hwaddr);

	hash_for_each_possible_rcu(sec->devices_hw, dev, bucket_hw, key) {
		if (dev->dev.hwaddr == hwaddr)
			return dev;
	}

	return NULL;
}