static u32 *ipv6_cow_metrics(struct dst_entry *dst, unsigned long old)
{
	struct rt6_info *rt = (struct rt6_info *) dst;
	struct inet_peer *peer;
	u32 *p = NULL;

	if (!(rt->dst.flags & DST_HOST))
		return NULL;

	peer = rt6_get_peer_create(rt);
	if (peer) {
		u32 *old_p = __DST_METRICS_PTR(old);
		unsigned long prev, new;

		p = peer->metrics;
		if (inet_metrics_new(peer) ||
		    (old & DST_METRICS_FORCE_OVERWRITE))
			memcpy(p, old_p, sizeof(u32) * RTAX_MAX);

		new = (unsigned long) p;
		prev = cmpxchg(&dst->_metrics, old, new);

		if (prev != old) {
			p = __DST_METRICS_PTR(prev);
			if (prev & DST_METRICS_READ_ONLY)
				p = NULL;
		}
	}
	return p;
}