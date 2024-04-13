static struct neighbour *ip6_neigh_lookup(const struct dst_entry *dst,
					  struct sk_buff *skb,
					  const void *daddr)
{
	struct rt6_info *rt = (struct rt6_info *) dst;
	struct neighbour *n;

	daddr = choose_neigh_daddr(rt, skb, daddr);
	n = __ipv6_neigh_lookup(dst->dev, daddr);
	if (n)
		return n;
	return neigh_create(&nd_tbl, daddr, dst->dev);
}