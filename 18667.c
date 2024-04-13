void xfrm_dst_ifdown(struct dst_entry *dst, struct net_device *dev)
{
	while ((dst = xfrm_dst_child(dst)) && dst->xfrm && dst->dev == dev) {
		dst->dev = blackhole_netdev;
		dev_hold(dst->dev);
		dev_put(dev);
	}
}