static struct rt6_info *rt6_select(struct fib6_node *fn, int oif, int strict)
{
	struct rt6_info *match, *rt0;
	struct net *net;
	bool do_rr = false;

	rt0 = fn->rr_ptr;
	if (!rt0)
		fn->rr_ptr = rt0 = fn->leaf;

	match = find_rr_leaf(fn, rt0, rt0->rt6i_metric, oif, strict,
			     &do_rr);

	if (do_rr) {
		struct rt6_info *next = rt0->dst.rt6_next;

		/* no entries matched; do round-robin */
		if (!next || next->rt6i_metric != rt0->rt6i_metric)
			next = fn->leaf;

		if (next != rt0)
			fn->rr_ptr = next;
	}

	net = dev_net(rt0->dst.dev);
	return match ? match : net->ipv6.ip6_null_entry;
}