static void ip6_dst_destroy(struct dst_entry *dst)
{
	struct rt6_info *rt = (struct rt6_info *)dst;
	struct inet6_dev *idev = rt->rt6i_idev;
	struct dst_entry *from = dst->from;

	if (!(rt->dst.flags & DST_HOST))
		dst_destroy_metrics_generic(dst);

	if (idev) {
		rt->rt6i_idev = NULL;
		in6_dev_put(idev);
	}

	dst->from = NULL;
	dst_release(from);

	if (rt6_has_peer(rt)) {
		struct inet_peer *peer = rt6_peer_ptr(rt);
		inet_putpeer(peer);
	}
}