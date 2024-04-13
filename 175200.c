nfqnl_recv_unsupp(struct sock *ctnl, struct sk_buff *skb,
		  const struct nlmsghdr *nlh,
		  const struct nlattr * const nfqa[])
{
	return -ENOTSUPP;
}