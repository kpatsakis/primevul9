void sock_init_data(struct socket *sock, struct sock *sk)
{
	sk_init_common(sk);
	sk->sk_send_head	=	NULL;

	init_timer(&sk->sk_timer);

	sk->sk_allocation	=	GFP_KERNEL;
	sk->sk_rcvbuf		=	sysctl_rmem_default;
	sk->sk_sndbuf		=	sysctl_wmem_default;
	sk->sk_state		=	TCP_CLOSE;
	sk_set_socket(sk, sock);

	sock_set_flag(sk, SOCK_ZAPPED);

	if (sock) {
		sk->sk_type	=	sock->type;
		sk->sk_wq	=	sock->wq;
		sock->sk	=	sk;
		sk->sk_uid	=	SOCK_INODE(sock)->i_uid;
	} else {
		sk->sk_wq	=	NULL;
		sk->sk_uid	=	make_kuid(sock_net(sk)->user_ns, 0);
	}

	rwlock_init(&sk->sk_callback_lock);
	if (sk->sk_kern_sock)
		lockdep_set_class_and_name(
			&sk->sk_callback_lock,
			af_kern_callback_keys + sk->sk_family,
			af_family_kern_clock_key_strings[sk->sk_family]);
	else
		lockdep_set_class_and_name(
			&sk->sk_callback_lock,
			af_callback_keys + sk->sk_family,
			af_family_clock_key_strings[sk->sk_family]);

	sk->sk_state_change	=	sock_def_wakeup;
	sk->sk_data_ready	=	sock_def_readable;
	sk->sk_write_space	=	sock_def_write_space;
	sk->sk_error_report	=	sock_def_error_report;
	sk->sk_destruct		=	sock_def_destruct;

	sk->sk_frag.page	=	NULL;
	sk->sk_frag.offset	=	0;
	sk->sk_peek_off		=	-1;

	sk->sk_peer_pid 	=	NULL;
	sk->sk_peer_cred	=	NULL;
	sk->sk_write_pending	=	0;
	sk->sk_rcvlowat		=	1;
	sk->sk_rcvtimeo		=	MAX_SCHEDULE_TIMEOUT;
	sk->sk_sndtimeo		=	MAX_SCHEDULE_TIMEOUT;

	sk->sk_stamp = SK_DEFAULT_STAMP;
	atomic_set(&sk->sk_zckey, 0);

#ifdef CONFIG_NET_RX_BUSY_POLL
	sk->sk_napi_id		=	0;
	sk->sk_ll_usec		=	sysctl_net_busy_read;
#endif

	sk->sk_max_pacing_rate = ~0U;
	sk->sk_pacing_rate = ~0U;
	sk->sk_incoming_cpu = -1;
	/*
	 * Before updating sk_refcnt, we must commit prior changes to memory
	 * (Documentation/RCU/rculist_nulls.txt for details)
	 */
	smp_wmb();
	refcount_set(&sk->sk_refcnt, 1);
	atomic_set(&sk->sk_drops, 0);
}