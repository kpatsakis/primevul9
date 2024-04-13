xfrm_policy_lookup_inexact_addr(const struct rb_root *r,
				seqcount_spinlock_t *count,
				const xfrm_address_t *addr, u16 family)
{
	const struct rb_node *parent;
	int seq;

again:
	seq = read_seqcount_begin(count);

	parent = rcu_dereference_raw(r->rb_node);
	while (parent) {
		struct xfrm_pol_inexact_node *node;
		int delta;

		node = rb_entry(parent, struct xfrm_pol_inexact_node, node);

		delta = xfrm_policy_addr_delta(addr, &node->addr,
					       node->prefixlen, family);
		if (delta < 0) {
			parent = rcu_dereference_raw(parent->rb_left);
			continue;
		} else if (delta > 0) {
			parent = rcu_dereference_raw(parent->rb_right);
			continue;
		}

		return node;
	}

	if (read_seqcount_retry(count, seq))
		goto again;

	return NULL;
}