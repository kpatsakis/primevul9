void ip_mc_drop_socket(struct sock *sk)
{
	struct inet_sock *inet = inet_sk(sk);
	struct ip_mc_socklist *iml;
	struct net *net = sock_net(sk);

	if (!inet->mc_list)
		return;

	rtnl_lock();
	while ((iml = rtnl_dereference(inet->mc_list)) != NULL) {
		struct in_device *in_dev;

		inet->mc_list = iml->next_rcu;
		in_dev = inetdev_by_index(net, iml->multi.imr_ifindex);
		(void) ip_mc_leave_src(sk, iml, in_dev);
		if (in_dev)
			ip_mc_dec_group(in_dev, iml->multi.imr_multiaddr.s_addr);
		/* decrease mem now to avoid the memleak warning */
		atomic_sub(sizeof(*iml), &sk->sk_omem_alloc);
		kfree_rcu(iml, rcu);
	}
	rtnl_unlock();
}