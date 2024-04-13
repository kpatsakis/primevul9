static int crypto_del_rng(struct sk_buff *skb, struct nlmsghdr *nlh,
			  struct nlattr **attrs)
{
	if (!netlink_capable(skb, CAP_NET_ADMIN))
		return -EPERM;
	return crypto_del_default_rng();
}