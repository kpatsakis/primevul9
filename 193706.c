int mac802154_llsec_dev_add(struct mac802154_llsec *sec,
			    const struct ieee802154_llsec_device *dev)
{
	struct mac802154_llsec_device *entry;
	u32 skey = llsec_dev_hash_short(dev->short_addr, dev->pan_id);
	u64 hwkey = llsec_dev_hash_long(dev->hwaddr);

	BUILD_BUG_ON(sizeof(hwkey) != IEEE802154_ADDR_LEN);

	if ((llsec_dev_use_shortaddr(dev->short_addr) &&
	     llsec_dev_find_short(sec, dev->short_addr, dev->pan_id)) ||
	     llsec_dev_find_long(sec, dev->hwaddr))
		return -EEXIST;

	entry = kmalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	entry->dev = *dev;
	spin_lock_init(&entry->lock);
	INIT_LIST_HEAD(&entry->dev.keys);

	if (llsec_dev_use_shortaddr(dev->short_addr))
		hash_add_rcu(sec->devices_short, &entry->bucket_s, skey);
	else
		INIT_HLIST_NODE(&entry->bucket_s);

	hash_add_rcu(sec->devices_hw, &entry->bucket_hw, hwkey);
	list_add_tail_rcu(&entry->dev.list, &sec->table.devices);

	return 0;
}