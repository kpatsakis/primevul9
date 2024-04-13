int mac802154_llsec_seclevel_del(struct mac802154_llsec *sec,
				 const struct ieee802154_llsec_seclevel *sl)
{
	struct mac802154_llsec_seclevel *pos;

	pos = llsec_find_seclevel(sec, sl);
	if (!pos)
		return -ENOENT;

	list_del_rcu(&pos->level.list);
	kfree_rcu(pos, rcu);

	return 0;
}