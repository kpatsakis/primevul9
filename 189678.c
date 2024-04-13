static int ping_v4_sendmsg(struct sock *sk, struct msghdr *msg, size_t len)
{
	struct net *net = sock_net(sk);
	struct flowi4 fl4;
	struct inet_sock *inet = inet_sk(sk);
	struct ipcm_cookie ipc;
	struct icmphdr user_icmph;
	struct pingfakehdr pfh;
	struct rtable *rt = NULL;
	struct ip_options_data opt_copy;
	int free = 0;
	__be32 saddr, daddr, faddr;
	u8  tos;
	int err;

	pr_debug("ping_v4_sendmsg(sk=%p,sk->num=%u)\n", inet, inet->inet_num);

	err = ping_common_sendmsg(AF_INET, msg, len, &user_icmph,
				  sizeof(user_icmph));
	if (err)
		return err;

	/*
	 *	Get and verify the address.
	 */

	if (msg->msg_name) {
		DECLARE_SOCKADDR(struct sockaddr_in *, usin, msg->msg_name);
		if (msg->msg_namelen < sizeof(*usin))
			return -EINVAL;
		if (usin->sin_family != AF_INET)
			return -EAFNOSUPPORT;
		daddr = usin->sin_addr.s_addr;
		/* no remote port */
	} else {
		if (sk->sk_state != TCP_ESTABLISHED)
			return -EDESTADDRREQ;
		daddr = inet->inet_daddr;
		/* no remote port */
	}

	ipc.sockc.tsflags = sk->sk_tsflags;
	ipc.addr = inet->inet_saddr;
	ipc.opt = NULL;
	ipc.oif = sk->sk_bound_dev_if;
	ipc.tx_flags = 0;
	ipc.ttl = 0;
	ipc.tos = -1;

	if (msg->msg_controllen) {
		err = ip_cmsg_send(sk, msg, &ipc, false);
		if (unlikely(err)) {
			kfree(ipc.opt);
			return err;
		}
		if (ipc.opt)
			free = 1;
	}
	if (!ipc.opt) {
		struct ip_options_rcu *inet_opt;

		rcu_read_lock();
		inet_opt = rcu_dereference(inet->inet_opt);
		if (inet_opt) {
			memcpy(&opt_copy, inet_opt,
			       sizeof(*inet_opt) + inet_opt->opt.optlen);
			ipc.opt = &opt_copy.opt;
		}
		rcu_read_unlock();
	}

	sock_tx_timestamp(sk, ipc.sockc.tsflags, &ipc.tx_flags);

	saddr = ipc.addr;
	ipc.addr = faddr = daddr;

	if (ipc.opt && ipc.opt->opt.srr) {
		if (!daddr)
			return -EINVAL;
		faddr = ipc.opt->opt.faddr;
	}
	tos = get_rttos(&ipc, inet);
	if (sock_flag(sk, SOCK_LOCALROUTE) ||
	    (msg->msg_flags & MSG_DONTROUTE) ||
	    (ipc.opt && ipc.opt->opt.is_strictroute)) {
		tos |= RTO_ONLINK;
	}

	if (ipv4_is_multicast(daddr)) {
		if (!ipc.oif)
			ipc.oif = inet->mc_index;
		if (!saddr)
			saddr = inet->mc_addr;
	} else if (!ipc.oif)
		ipc.oif = inet->uc_index;

	flowi4_init_output(&fl4, ipc.oif, sk->sk_mark, tos,
			   RT_SCOPE_UNIVERSE, sk->sk_protocol,
			   inet_sk_flowi_flags(sk), faddr, saddr, 0, 0,
			   sk->sk_uid);

	security_sk_classify_flow(sk, flowi4_to_flowi(&fl4));
	rt = ip_route_output_flow(net, &fl4, sk);
	if (IS_ERR(rt)) {
		err = PTR_ERR(rt);
		rt = NULL;
		if (err == -ENETUNREACH)
			IP_INC_STATS(net, IPSTATS_MIB_OUTNOROUTES);
		goto out;
	}

	err = -EACCES;
	if ((rt->rt_flags & RTCF_BROADCAST) &&
	    !sock_flag(sk, SOCK_BROADCAST))
		goto out;

	if (msg->msg_flags & MSG_CONFIRM)
		goto do_confirm;
back_from_confirm:

	if (!ipc.addr)
		ipc.addr = fl4.daddr;

	lock_sock(sk);

	pfh.icmph.type = user_icmph.type; /* already checked */
	pfh.icmph.code = user_icmph.code; /* ditto */
	pfh.icmph.checksum = 0;
	pfh.icmph.un.echo.id = inet->inet_sport;
	pfh.icmph.un.echo.sequence = user_icmph.un.echo.sequence;
	pfh.msg = msg;
	pfh.wcheck = 0;
	pfh.family = AF_INET;

	err = ip_append_data(sk, &fl4, ping_getfrag, &pfh, len,
			0, &ipc, &rt, msg->msg_flags);
	if (err)
		ip_flush_pending_frames(sk);
	else
		err = ping_v4_push_pending_frames(sk, &pfh, &fl4);
	release_sock(sk);

out:
	ip_rt_put(rt);
	if (free)
		kfree(ipc.opt);
	if (!err) {
		icmp_out_count(sock_net(sk), user_icmph.type);
		return len;
	}
	return err;

do_confirm:
	if (msg->msg_flags & MSG_PROBE)
		dst_confirm_neigh(&rt->dst, &fl4.daddr);
	if (!(msg->msg_flags & MSG_PROBE) || len)
		goto back_from_confirm;
	err = 0;
	goto out;
}