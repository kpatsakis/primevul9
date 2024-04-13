static void rt6_bind_peer(struct rt6_info *rt, int create)
{
	struct inet_peer_base *base;
	struct inet_peer *peer;

	base = inetpeer_base_ptr(rt->_rt6i_peer);
	if (!base)
		return;

	peer = inet_getpeer_v6(base, &rt->rt6i_dst.addr, create);
	if (peer) {
		if (!rt6_set_peer(rt, peer))
			inet_putpeer(peer);
	}
}