static void xfrm_bydst_resize(struct net *net, int dir)
{
	unsigned int hmask = net->xfrm.policy_bydst[dir].hmask;
	unsigned int nhashmask = xfrm_new_hash_mask(hmask);
	unsigned int nsize = (nhashmask + 1) * sizeof(struct hlist_head);
	struct hlist_head *ndst = xfrm_hash_alloc(nsize);
	struct hlist_head *odst;
	int i;

	if (!ndst)
		return;

	spin_lock_bh(&net->xfrm.xfrm_policy_lock);
	write_seqcount_begin(&net->xfrm.xfrm_policy_hash_generation);

	odst = rcu_dereference_protected(net->xfrm.policy_bydst[dir].table,
				lockdep_is_held(&net->xfrm.xfrm_policy_lock));

	for (i = hmask; i >= 0; i--)
		xfrm_dst_hash_transfer(net, odst + i, ndst, nhashmask, dir);

	rcu_assign_pointer(net->xfrm.policy_bydst[dir].table, ndst);
	net->xfrm.policy_bydst[dir].hmask = nhashmask;

	write_seqcount_end(&net->xfrm.xfrm_policy_hash_generation);
	spin_unlock_bh(&net->xfrm.xfrm_policy_lock);

	synchronize_rcu();

	xfrm_hash_free(odst, (hmask + 1) * sizeof(struct hlist_head));
}