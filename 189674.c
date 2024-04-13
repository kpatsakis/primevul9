int ping_bind(struct sock *sk, struct sockaddr *uaddr, int addr_len)
{
	struct inet_sock *isk = inet_sk(sk);
	unsigned short snum;
	int err;
	int dif = sk->sk_bound_dev_if;

	err = ping_check_bind_addr(sk, isk, uaddr, addr_len);
	if (err)
		return err;

	lock_sock(sk);

	err = -EINVAL;
	if (isk->inet_num != 0)
		goto out;

	err = -EADDRINUSE;
	ping_set_saddr(sk, uaddr);
	snum = ntohs(((struct sockaddr_in *)uaddr)->sin_port);
	if (ping_get_port(sk, snum) != 0) {
		ping_clear_saddr(sk, dif);
		goto out;
	}

	pr_debug("after bind(): num = %hu, dif = %d\n",
		 isk->inet_num,
		 sk->sk_bound_dev_if);

	err = 0;
	if (sk->sk_family == AF_INET && isk->inet_rcv_saddr)
		sk->sk_userlocks |= SOCK_BINDADDR_LOCK;
#if IS_ENABLED(CONFIG_IPV6)
	if (sk->sk_family == AF_INET6 && !ipv6_addr_any(&sk->sk_v6_rcv_saddr))
		sk->sk_userlocks |= SOCK_BINDADDR_LOCK;
#endif

	if (snum)
		sk->sk_userlocks |= SOCK_BINDPORT_LOCK;
	isk->inet_sport = htons(isk->inet_num);
	isk->inet_daddr = 0;
	isk->inet_dport = 0;

#if IS_ENABLED(CONFIG_IPV6)
	if (sk->sk_family == AF_INET6)
		memset(&sk->sk_v6_daddr, 0, sizeof(sk->sk_v6_daddr));
#endif

	sk_dst_reset(sk);
out:
	release_sock(sk);
	pr_debug("ping_v4_bind -> %d\n", err);
	return err;
}