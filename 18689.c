static void __xfrm_policy_inexact_prune_bin(struct xfrm_pol_inexact_bin *b, bool net_exit)
{
	write_seqcount_begin(&b->count);
	xfrm_policy_inexact_gc_tree(&b->root_d, net_exit);
	xfrm_policy_inexact_gc_tree(&b->root_s, net_exit);
	write_seqcount_end(&b->count);

	if (!RB_EMPTY_ROOT(&b->root_d) || !RB_EMPTY_ROOT(&b->root_s) ||
	    !hlist_empty(&b->hhead)) {
		WARN_ON_ONCE(net_exit);
		return;
	}

	if (rhashtable_remove_fast(&xfrm_policy_inexact_table, &b->head,
				   xfrm_pol_inexact_params) == 0) {
		list_del(&b->inexact_bins);
		kfree_rcu(b, rcu);
	}
}