llsec_dev_find_short(struct mac802154_llsec *sec, __le16 short_addr,
		     __le16 pan_id)
{
	struct mac802154_llsec_device *dev;
	u32 key = llsec_dev_hash_short(short_addr, pan_id);

	hash_for_each_possible_rcu(sec->devices_short, dev, bucket_s, key) {
		if (dev->dev.short_addr == short_addr &&
		    dev->dev.pan_id == pan_id)
			return dev;
	}

	return NULL;
}