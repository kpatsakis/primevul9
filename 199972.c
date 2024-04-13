void dccp_set_state(struct sock *sk, const int state)
{
	const int oldstate = sk->sk_state;

	dccp_pr_debug("%s(%p) %-10.10s -> %s\n",
		      dccp_role(sk), sk,
		      dccp_state_name(oldstate), dccp_state_name(state));
	WARN_ON(state == oldstate);

	switch (state) {
	case DCCP_OPEN:
		if (oldstate != DCCP_OPEN)
			DCCP_INC_STATS(DCCP_MIB_CURRESTAB);
		break;

	case DCCP_CLOSED:
		if (oldstate == DCCP_CLOSING || oldstate == DCCP_OPEN)
			DCCP_INC_STATS(DCCP_MIB_ESTABRESETS);

		sk->sk_prot->unhash(sk);
		if (inet_csk(sk)->icsk_bind_hash != NULL &&
		    !(sk->sk_userlocks & SOCK_BINDPORT_LOCK))
			inet_put_port(&dccp_hashinfo, sk);
		/* fall through */
	default:
		if (oldstate == DCCP_OPEN)
			DCCP_DEC_STATS(DCCP_MIB_CURRESTAB);
	}

	/* Change state AFTER socket is unhashed to avoid closed
	 * socket sitting in hash tables.
	 */
	sk->sk_state = state;
}