static void xfrm_policy_inexact_node_reinsert(struct net *net,
					      struct xfrm_pol_inexact_node *n,
					      struct rb_root *new,
					      u16 family)
{
	struct xfrm_pol_inexact_node *node;
	struct rb_node **p, *parent;

	/* we should not have another subtree here */
	WARN_ON_ONCE(!RB_EMPTY_ROOT(&n->root));
restart:
	parent = NULL;
	p = &new->rb_node;
	while (*p) {
		u8 prefixlen;
		int delta;

		parent = *p;
		node = rb_entry(*p, struct xfrm_pol_inexact_node, node);

		prefixlen = min(node->prefixlen, n->prefixlen);

		delta = xfrm_policy_addr_delta(&n->addr, &node->addr,
					       prefixlen, family);
		if (delta < 0) {
			p = &parent->rb_left;
		} else if (delta > 0) {
			p = &parent->rb_right;
		} else {
			bool same_prefixlen = node->prefixlen == n->prefixlen;
			struct xfrm_policy *tmp;

			hlist_for_each_entry(tmp, &n->hhead, bydst) {
				tmp->bydst_reinsert = true;
				hlist_del_rcu(&tmp->bydst);
			}

			node->prefixlen = prefixlen;

			xfrm_policy_inexact_list_reinsert(net, node, family);

			if (same_prefixlen) {
				kfree_rcu(n, rcu);
				return;
			}

			rb_erase(*p, new);
			kfree_rcu(n, rcu);
			n = node;
			goto restart;
		}
	}

	rb_link_node_rcu(&n->node, parent, p);
	rb_insert_color(&n->node, new);
}