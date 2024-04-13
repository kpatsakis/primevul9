void mac802154_llsec_init(struct mac802154_llsec *sec)
{
	memset(sec, 0, sizeof(*sec));

	memset(&sec->params.default_key_source, 0xFF, IEEE802154_ADDR_LEN);

	INIT_LIST_HEAD(&sec->table.security_levels);
	INIT_LIST_HEAD(&sec->table.devices);
	INIT_LIST_HEAD(&sec->table.keys);
	hash_init(sec->devices_short);
	hash_init(sec->devices_hw);
	rwlock_init(&sec->lock);
}