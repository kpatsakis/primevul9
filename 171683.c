void sock_enable_timestamp(struct sock *sk, int flag)
{
	if (!sock_flag(sk, flag)) {
		unsigned long previous_flags = sk->sk_flags;

		sock_set_flag(sk, flag);
		/*
		 * we just set one of the two flags which require net
		 * time stamping, but time stamping might have been on
		 * already because of the other one
		 */
		if (sock_needs_netstamp(sk) &&
		    !(previous_flags & SK_FLAGS_TIMESTAMP))
			net_enable_timestamp();
	}
}