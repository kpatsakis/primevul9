xfrm_policy_inexact_insert_node(struct net *net,
				struct rb_root *root,
				xfrm_address_t *addr,
				u16 family, u8 prefixlen, u8 dir)
{
	struct xfrm_pol_inexact_node *cached = NULL;
	struct rb_node **p, *parent = NULL;
	struct xfrm_pol_inexact_node *node;

	p = &root->rb_node;
	while (*p) {
		int delta;

		parent = *p;
		node = rb_entry(*p, struct xfrm_pol_inexact_node, node);

		delta = xfrm_policy_addr_delta(addr, &node->addr,
					       node->prefixlen,
					       family);
		if (delta == 0 && prefixlen >= node->prefixlen) {
			WARN_ON_ONCE(cached); /* ipsec policies got lost */
			return node;
		}

		if (delta < 0)
			p = &parent->rb_left;
		else
			p = &parent->rb_right;

		if (prefixlen < node->prefixlen) {
			delta = xfrm_policy_addr_delta(addr, &node->addr,
						       prefixlen,
						       family);
			if (delta)
				continue;

			/* This node is a subnet of the new prefix. It needs
			 * to be removed and re-inserted with the smaller
			 * prefix and all nodes that are now also covered
			 * by the reduced prefixlen.
			 */
			rb_erase(&node->node, root);

			if (!cached) {
				xfrm_pol_inexact_node_init(node, addr,
							   prefixlen);
				cached = node;
			} else {
				/* This node also falls within the new
				 * prefixlen. Merge the to-be-reinserted
				 * node and this one.
				 */
				xfrm_policy_inexact_node_merge(net, node,
							       cached, family);
				kfree_rcu(node, rcu);
			}

			/* restart */
			p = &root->rb_node;
			parent = NULL;
		}
	}

	node = cached;
	if (!node) {
		node = xfrm_pol_inexact_node_alloc(addr, prefixlen);
		if (!node)
			return NULL;
	}

	rb_link_node_rcu(&node->node, parent, p);
	rb_insert_color(&node->node, root);

	return node;
}