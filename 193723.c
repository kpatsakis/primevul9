llsec_lookup_dev(struct mac802154_llsec *sec,
		 const struct ieee802154_addr *addr)
{
	struct ieee802154_addr devaddr = *addr;
	struct mac802154_llsec_device *dev = NULL;

	if (devaddr.mode == IEEE802154_ADDR_NONE &&
	    llsec_recover_addr(sec, &devaddr) < 0)
		return NULL;

	if (devaddr.mode == IEEE802154_ADDR_SHORT) {
		u32 key = llsec_dev_hash_short(devaddr.short_addr,
					       devaddr.pan_id);

		hash_for_each_possible_rcu(sec->devices_short, dev,
					   bucket_s, key) {
			if (dev->dev.pan_id == devaddr.pan_id &&
			    dev->dev.short_addr == devaddr.short_addr)
				return dev;
		}
	} else {
		u64 key = llsec_dev_hash_long(devaddr.extended_addr);

		hash_for_each_possible_rcu(sec->devices_hw, dev,
					   bucket_hw, key) {
			if (dev->dev.hwaddr == devaddr.extended_addr)
				return dev;
		}
	}

	return NULL;
}