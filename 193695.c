int mac802154_llsec_dev_del(struct mac802154_llsec *sec, __le64 device_addr)
{
	struct mac802154_llsec_device *pos;

	pos = llsec_dev_find_long(sec, device_addr);
	if (!pos)
		return -ENOENT;

	hash_del_rcu(&pos->bucket_s);
	hash_del_rcu(&pos->bucket_hw);
	list_del_rcu(&pos->dev.list);
	call_rcu(&pos->rcu, llsec_dev_free_rcu);

	return 0;
}