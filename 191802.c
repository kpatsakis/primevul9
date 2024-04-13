static int netlink_getname(struct socket *sock, struct sockaddr *addr,
			   int *addr_len, int peer)
{
	struct sock *sk = sock->sk;
	struct netlink_sock *nlk = nlk_sk(sk);
	DECLARE_SOCKADDR(struct sockaddr_nl *, nladdr, addr);

	nladdr->nl_family = AF_NETLINK;
	nladdr->nl_pad = 0;
	*addr_len = sizeof(*nladdr);

	if (peer) {
		nladdr->nl_pid = nlk->dst_portid;
		nladdr->nl_groups = netlink_group_mask(nlk->dst_group);
	} else {
		nladdr->nl_pid = nlk->portid;
		netlink_lock_table();
		nladdr->nl_groups = nlk->groups ? nlk->groups[0] : 0;
		netlink_unlock_table();
	}
	return 0;
}