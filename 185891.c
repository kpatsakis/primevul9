static void ip6_dst_ifdown(struct dst_entry *dst, struct net_device *dev,
			   int how)
{
	struct rt6_info *rt = (struct rt6_info *)dst;
	struct inet6_dev *idev = rt->rt6i_idev;
	struct net_device *loopback_dev =
		dev_net(dev)->loopback_dev;

	if (dev != loopback_dev) {
		if (idev && idev->dev == dev) {
			struct inet6_dev *loopback_idev =
				in6_dev_get(loopback_dev);
			if (loopback_idev) {
				rt->rt6i_idev = loopback_idev;
				in6_dev_put(idev);
			}
		}
	}
}