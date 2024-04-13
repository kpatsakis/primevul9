static int ping_check_bind_addr(struct sock *sk, struct inet_sock *isk,
				struct sockaddr *uaddr, int addr_len) {
	struct net *net = sock_net(sk);
	if (sk->sk_family == AF_INET) {
		struct sockaddr_in *addr = (struct sockaddr_in *) uaddr;
		int chk_addr_ret;

		if (addr_len < sizeof(*addr))
			return -EINVAL;

		pr_debug("ping_check_bind_addr(sk=%p,addr=%pI4,port=%d)\n",
			 sk, &addr->sin_addr.s_addr, ntohs(addr->sin_port));

		chk_addr_ret = inet_addr_type(net, addr->sin_addr.s_addr);

		if (addr->sin_addr.s_addr == htonl(INADDR_ANY))
			chk_addr_ret = RTN_LOCAL;

		if ((sysctl_ip_nonlocal_bind == 0 &&
		    isk->freebind == 0 && isk->transparent == 0 &&
		     chk_addr_ret != RTN_LOCAL) ||
		    chk_addr_ret == RTN_MULTICAST ||
		    chk_addr_ret == RTN_BROADCAST)
			return -EADDRNOTAVAIL;

#if IS_ENABLED(CONFIG_IPV6)
	} else if (sk->sk_family == AF_INET6) {
		struct sockaddr_in6 *addr = (struct sockaddr_in6 *) uaddr;
		int addr_type, scoped, has_addr;
		struct net_device *dev = NULL;

		if (addr_len < sizeof(*addr))
			return -EINVAL;

		if (addr->sin6_family != AF_INET6)
			return -EINVAL;

		pr_debug("ping_check_bind_addr(sk=%p,addr=%pI6c,port=%d)\n",
			 sk, addr->sin6_addr.s6_addr, ntohs(addr->sin6_port));

		addr_type = ipv6_addr_type(&addr->sin6_addr);
		scoped = __ipv6_addr_needs_scope_id(addr_type);
		if ((addr_type != IPV6_ADDR_ANY &&
		     !(addr_type & IPV6_ADDR_UNICAST)) ||
		    (scoped && !addr->sin6_scope_id))
			return -EINVAL;

		rcu_read_lock();
		if (addr->sin6_scope_id) {
			dev = dev_get_by_index_rcu(net, addr->sin6_scope_id);
			if (!dev) {
				rcu_read_unlock();
				return -ENODEV;
			}
		}
		has_addr = pingv6_ops.ipv6_chk_addr(net, &addr->sin6_addr, dev,
						    scoped);
		rcu_read_unlock();

		if (!(isk->freebind || isk->transparent || has_addr ||
		      addr_type == IPV6_ADDR_ANY))
			return -EADDRNOTAVAIL;

		if (scoped)
			sk->sk_bound_dev_if = addr->sin6_scope_id;
#endif
	} else {
		return -EAFNOSUPPORT;
	}
	return 0;
}