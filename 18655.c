static struct hlist_head *policy_hash_bysel(struct net *net,
					    const struct xfrm_selector *sel,
					    unsigned short family, int dir)
{
	unsigned int hmask = net->xfrm.policy_bydst[dir].hmask;
	unsigned int hash;
	u8 dbits;
	u8 sbits;

	__get_hash_thresh(net, family, dir, &dbits, &sbits);
	hash = __sel_hash(sel, family, hmask, dbits, sbits);

	if (hash == hmask + 1)
		return NULL;

	return rcu_dereference_check(net->xfrm.policy_bydst[dir].table,
		     lockdep_is_held(&net->xfrm.xfrm_policy_lock)) + hash;
}