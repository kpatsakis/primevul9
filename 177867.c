data_sock_release(struct socket *sock)
{
	struct sock *sk = sock->sk;

	if (*debug & DEBUG_SOCKET)
		printk(KERN_DEBUG "%s(%p) sk=%p\n", __func__, sock, sk);
	if (!sk)
		return 0;
	switch (sk->sk_protocol) {
	case ISDN_P_TE_S0:
	case ISDN_P_NT_S0:
	case ISDN_P_TE_E1:
	case ISDN_P_NT_E1:
		if (sk->sk_state == MISDN_BOUND)
			delete_channel(&_pms(sk)->ch);
		else
			mISDN_sock_unlink(&data_sockets, sk);
		break;
	case ISDN_P_LAPD_TE:
	case ISDN_P_LAPD_NT:
	case ISDN_P_B_RAW:
	case ISDN_P_B_HDLC:
	case ISDN_P_B_X75SLP:
	case ISDN_P_B_L2DTMF:
	case ISDN_P_B_L2DSP:
	case ISDN_P_B_L2DSPHDLC:
		delete_channel(&_pms(sk)->ch);
		mISDN_sock_unlink(&data_sockets, sk);
		break;
	}

	lock_sock(sk);

	sock_orphan(sk);
	skb_queue_purge(&sk->sk_receive_queue);

	release_sock(sk);
	sock_put(sk);

	return 0;
}