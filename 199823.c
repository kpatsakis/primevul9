static void inet6_ifa_notify(int event, struct inet6_ifaddr *ifa)
{
	struct sk_buff *skb;
	struct net *net = dev_net(ifa->idev->dev);
	int err = -ENOBUFS;

	skb = nlmsg_new(inet6_ifaddr_msgsize(), GFP_ATOMIC);
	if (skb == NULL)
		goto errout;

	err = inet6_fill_ifaddr(skb, ifa, 0, 0, event, 0);
	if (err < 0) {
		/* -EMSGSIZE implies BUG in inet6_ifaddr_msgsize() */
		WARN_ON(err == -EMSGSIZE);
		kfree_skb(skb);
		goto errout;
	}
	rtnl_notify(skb, net, 0, RTNLGRP_IPV6_IFADDR, NULL, GFP_ATOMIC);
	return;
errout:
	if (err < 0)
		rtnl_set_sk_err(net, RTNLGRP_IPV6_IFADDR, err);
}