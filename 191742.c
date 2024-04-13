static void netlink_cmsg_listen_all_nsid(struct sock *sk, struct msghdr *msg,
					 struct sk_buff *skb)
{
	if (!NETLINK_CB(skb).nsid_is_set)
		return;

	put_cmsg(msg, SOL_NETLINK, NETLINK_LISTEN_ALL_NSID, sizeof(int),
		 &NETLINK_CB(skb).nsid);
}