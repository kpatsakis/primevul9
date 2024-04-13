int mac802154_llsec_seclevel_add(struct mac802154_llsec *sec,
				 const struct ieee802154_llsec_seclevel *sl)
{
	struct mac802154_llsec_seclevel *entry;

	if (llsec_find_seclevel(sec, sl))
		return -EEXIST;

	entry = kmalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	entry->level = *sl;

	list_add_tail_rcu(&entry->level.list, &sec->table.security_levels);

	return 0;
}