static struct inet_peer *__rt6_get_peer(struct rt6_info *rt, int create)
{
	if (rt6_has_peer(rt))
		return rt6_peer_ptr(rt);

	rt6_bind_peer(rt, create);
	return (rt6_has_peer(rt) ? rt6_peer_ptr(rt) : NULL);
}