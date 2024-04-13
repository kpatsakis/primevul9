static int inet6_fill_link_af(struct sk_buff *skb, const struct net_device *dev)
{
	struct inet6_dev *idev = __in6_dev_get(dev);

	if (!idev)
		return -ENODATA;

	if (inet6_fill_ifla6_attrs(skb, idev) < 0)
		return -EMSGSIZE;

	return 0;
}