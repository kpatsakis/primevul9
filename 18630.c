static void xfrm_policy_inexact_gc_tree(struct rb_root *r, bool rm)
{
	struct xfrm_pol_inexact_node *node;
	struct rb_node *rn = rb_first(r);

	while (rn) {
		node = rb_entry(rn, struct xfrm_pol_inexact_node, node);

		xfrm_policy_inexact_gc_tree(&node->root, rm);
		rn = rb_next(rn);

		if (!hlist_empty(&node->hhead) || !RB_EMPTY_ROOT(&node->root)) {
			WARN_ON_ONCE(rm);
			continue;
		}

		rb_erase(&node->node, r);
		kfree_rcu(node, rcu);
	}
}