static int nf_tables_getgen(struct sock *nlsk, struct sk_buff *skb,
			    const struct nlmsghdr *nlh,
			    const struct nlattr * const nla[])
{
	struct net *net = sock_net(skb->sk);
	struct sk_buff *skb2;
	int err;

	skb2 = alloc_skb(NLMSG_GOODSIZE, GFP_KERNEL);
	if (skb2 == NULL)
		return -ENOMEM;

	err = nf_tables_fill_gen_info(skb2, net, NETLINK_CB(skb).portid,
				      nlh->nlmsg_seq);
	if (err < 0)
		goto err;

	return nlmsg_unicast(nlsk, skb2, NETLINK_CB(skb).portid);
err:
	kfree_skb(skb2);
	return err;
}