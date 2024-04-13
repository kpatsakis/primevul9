static int __net_init xfrm_policy_init(struct net *net)
{
	unsigned int hmask, sz;
	int dir, err;

	if (net_eq(net, &init_net)) {
		xfrm_dst_cache = kmem_cache_create("xfrm_dst_cache",
					   sizeof(struct xfrm_dst),
					   0, SLAB_HWCACHE_ALIGN|SLAB_PANIC,
					   NULL);
		err = rhashtable_init(&xfrm_policy_inexact_table,
				      &xfrm_pol_inexact_params);
		BUG_ON(err);
	}

	hmask = 8 - 1;
	sz = (hmask+1) * sizeof(struct hlist_head);

	net->xfrm.policy_byidx = xfrm_hash_alloc(sz);
	if (!net->xfrm.policy_byidx)
		goto out_byidx;
	net->xfrm.policy_idx_hmask = hmask;

	for (dir = 0; dir < XFRM_POLICY_MAX; dir++) {
		struct xfrm_policy_hash *htab;

		net->xfrm.policy_count[dir] = 0;
		net->xfrm.policy_count[XFRM_POLICY_MAX + dir] = 0;
		INIT_HLIST_HEAD(&net->xfrm.policy_inexact[dir]);

		htab = &net->xfrm.policy_bydst[dir];
		htab->table = xfrm_hash_alloc(sz);
		if (!htab->table)
			goto out_bydst;
		htab->hmask = hmask;
		htab->dbits4 = 32;
		htab->sbits4 = 32;
		htab->dbits6 = 128;
		htab->sbits6 = 128;
	}
	net->xfrm.policy_hthresh.lbits4 = 32;
	net->xfrm.policy_hthresh.rbits4 = 32;
	net->xfrm.policy_hthresh.lbits6 = 128;
	net->xfrm.policy_hthresh.rbits6 = 128;

	seqlock_init(&net->xfrm.policy_hthresh.lock);

	INIT_LIST_HEAD(&net->xfrm.policy_all);
	INIT_LIST_HEAD(&net->xfrm.inexact_bins);
	INIT_WORK(&net->xfrm.policy_hash_work, xfrm_hash_resize);
	INIT_WORK(&net->xfrm.policy_hthresh.work, xfrm_hash_rebuild);
	return 0;

out_bydst:
	for (dir--; dir >= 0; dir--) {
		struct xfrm_policy_hash *htab;

		htab = &net->xfrm.policy_bydst[dir];
		xfrm_hash_free(htab->table, sz);
	}
	xfrm_hash_free(net->xfrm.policy_byidx, sz);
out_byidx:
	return -ENOMEM;
}