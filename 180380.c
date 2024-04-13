static void dccp_finish_passive_close(struct sock *sk)
{
	switch (sk->sk_state) {
	case DCCP_PASSIVE_CLOSE:
		/* Node (client or server) has received Close packet. */
		dccp_send_reset(sk, DCCP_RESET_CODE_CLOSED);
		dccp_set_state(sk, DCCP_CLOSED);
		break;
	case DCCP_PASSIVE_CLOSEREQ:
		/*
		 * Client received CloseReq. We set the `active' flag so that
		 * dccp_send_close() retransmits the Close as per RFC 4340, 8.3.
		 */
		dccp_send_close(sk, 1);
		dccp_set_state(sk, DCCP_CLOSING);
	}
}