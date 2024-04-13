static void dccp_terminate_connection(struct sock *sk)
{
	u8 next_state = DCCP_CLOSED;

	switch (sk->sk_state) {
	case DCCP_PASSIVE_CLOSE:
	case DCCP_PASSIVE_CLOSEREQ:
		dccp_finish_passive_close(sk);
		break;
	case DCCP_PARTOPEN:
		dccp_pr_debug("Stop PARTOPEN timer (%p)\n", sk);
		inet_csk_clear_xmit_timer(sk, ICSK_TIME_DACK);
		/* fall through */
	case DCCP_OPEN:
		dccp_send_close(sk, 1);

		if (dccp_sk(sk)->dccps_role == DCCP_ROLE_SERVER &&
		    !dccp_sk(sk)->dccps_server_timewait)
			next_state = DCCP_ACTIVE_CLOSEREQ;
		else
			next_state = DCCP_CLOSING;
		/* fall through */
	default:
		dccp_set_state(sk, next_state);
	}
}