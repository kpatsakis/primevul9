void dccp_set_state(struct sock *sk, const int state)
{
	const int oldstate = sk->sk_state;

	dccp_pr_debug("%s(%p)  %s  -->  %s\n", dccp_role(sk), sk,
		      dccp_state_name(oldstate), dccp_state_name(state));
	WARN_ON(state == oldstate);

	switch (state) {
	case DCCP_OPEN:
		if (oldstate != DCCP_OPEN)
			DCCP_INC_STATS(DCCP_MIB_CURRESTAB);
		/* Client retransmits all Confirm options until entering OPEN */
		if (oldstate == DCCP_PARTOPEN)
			dccp_feat_list_purge(&dccp_sk(sk)->dccps_featneg);
		break;

	case DCCP_CLOSED:
		if (oldstate == DCCP_OPEN || oldstate == DCCP_ACTIVE_CLOSEREQ ||
		    oldstate == DCCP_CLOSING)
			DCCP_INC_STATS(DCCP_MIB_ESTABRESETS);

		sk->sk_prot->unhash(sk);
		if (inet_csk(sk)->icsk_bind_hash != NULL &&
		    !(sk->sk_userlocks & SOCK_BINDPORT_LOCK))
			inet_put_port(sk);
		/* fall through */
	default:
		if (oldstate == DCCP_OPEN)
			DCCP_DEC_STATS(DCCP_MIB_CURRESTAB);
	}

	/* Change state AFTER socket is unhashed to avoid closed
	 * socket sitting in hash tables.
	 */
	inet_sk_set_state(sk, state);
}