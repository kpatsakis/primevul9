static void netlink_overrun(struct sock *sk)
{
	struct netlink_sock *nlk = nlk_sk(sk);

	if (!(nlk->flags & NETLINK_F_RECV_NO_ENOBUFS)) {
		if (!test_and_set_bit(NETLINK_S_CONGESTED,
				      &nlk_sk(sk)->state)) {
			sk->sk_err = ENOBUFS;
			sk->sk_error_report(sk);
		}
	}
	atomic_inc(&sk->sk_drops);
}