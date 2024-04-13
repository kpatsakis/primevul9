xfrm_policy_inexact_alloc_bin(const struct xfrm_policy *pol, u8 dir)
{
	struct xfrm_pol_inexact_bin *bin, *prev;
	struct xfrm_pol_inexact_key k = {
		.family = pol->family,
		.type = pol->type,
		.dir = dir,
		.if_id = pol->if_id,
	};
	struct net *net = xp_net(pol);

	lockdep_assert_held(&net->xfrm.xfrm_policy_lock);

	write_pnet(&k.net, net);
	bin = rhashtable_lookup_fast(&xfrm_policy_inexact_table, &k,
				     xfrm_pol_inexact_params);
	if (bin)
		return bin;

	bin = kzalloc(sizeof(*bin), GFP_ATOMIC);
	if (!bin)
		return NULL;

	bin->k = k;
	INIT_HLIST_HEAD(&bin->hhead);
	bin->root_d = RB_ROOT;
	bin->root_s = RB_ROOT;
	seqcount_spinlock_init(&bin->count, &net->xfrm.xfrm_policy_lock);

	prev = rhashtable_lookup_get_insert_key(&xfrm_policy_inexact_table,
						&bin->k, &bin->head,
						xfrm_pol_inexact_params);
	if (!prev) {
		list_add(&bin->inexact_bins, &net->xfrm.inexact_bins);
		return bin;
	}

	kfree(bin);

	return IS_ERR(prev) ? NULL : prev;
}