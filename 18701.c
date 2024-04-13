static void xfrm_policy_inexact_node_merge(struct net *net,
					   struct xfrm_pol_inexact_node *v,
					   struct xfrm_pol_inexact_node *n,
					   u16 family)
{
	struct xfrm_pol_inexact_node *node;
	struct xfrm_policy *tmp;
	struct rb_node *rnode;

	/* To-be-merged node v has a subtree.
	 *
	 * Dismantle it and insert its nodes to n->root.
	 */
	while ((rnode = rb_first(&v->root)) != NULL) {
		node = rb_entry(rnode, struct xfrm_pol_inexact_node, node);
		rb_erase(&node->node, &v->root);
		xfrm_policy_inexact_node_reinsert(net, node, &n->root,
						  family);
	}

	hlist_for_each_entry(tmp, &v->hhead, bydst) {
		tmp->bydst_reinsert = true;
		hlist_del_rcu(&tmp->bydst);
	}

	xfrm_policy_inexact_list_reinsert(net, n, family);
}