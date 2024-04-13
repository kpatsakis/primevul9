static struct inet_peer *rt6_get_peer_create(struct rt6_info *rt)
{
	return __rt6_get_peer(rt, 1);
}