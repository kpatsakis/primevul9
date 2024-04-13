static struct neighbour *xfrm_neigh_lookup(const struct dst_entry *dst,
					   struct sk_buff *skb,
					   const void *daddr)
{
	const struct dst_entry *path = xfrm_dst_path(dst);

	if (!skb)
		daddr = xfrm_get_dst_nexthop(dst, daddr);
	return path->ops->neigh_lookup(path, skb, daddr);
}