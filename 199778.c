void inet6_netconf_notify_devconf(struct net *net, int type, int ifindex,
				  struct ipv6_devconf *devconf)
{
	struct sk_buff *skb;
	int err = -ENOBUFS;

	skb = nlmsg_new(inet6_netconf_msgsize_devconf(type), GFP_ATOMIC);
	if (skb == NULL)
		goto errout;

	err = inet6_netconf_fill_devconf(skb, ifindex, devconf, 0, 0,
					 RTM_NEWNETCONF, 0, type);
	if (err < 0) {
		/* -EMSGSIZE implies BUG in inet6_netconf_msgsize_devconf() */
		WARN_ON(err == -EMSGSIZE);
		kfree_skb(skb);
		goto errout;
	}
	rtnl_notify(skb, net, 0, RTNLGRP_IPV6_NETCONF, NULL, GFP_ATOMIC);
	return;
errout:
	rtnl_set_sk_err(net, RTNLGRP_IPV6_NETCONF, err);
}