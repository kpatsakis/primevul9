static void tcp_fastopen_synack_timer(struct sock *sk)
{
	struct inet_connection_sock *icsk = inet_csk(sk);
	int max_retries = icsk->icsk_syn_retries ? :
	    sock_net(sk)->ipv4.sysctl_tcp_synack_retries + 1; /* add one more retry for fastopen */
	struct tcp_sock *tp = tcp_sk(sk);
	struct request_sock *req;

	req = tcp_sk(sk)->fastopen_rsk;
	req->rsk_ops->syn_ack_timeout(req);

	if (req->num_timeout >= max_retries) {
		tcp_write_err(sk);
		return;
	}
	/* Lower cwnd after certain SYNACK timeout like tcp_init_transfer() */
	if (icsk->icsk_retransmits == 1)
		tcp_enter_loss(sk);
	/* XXX (TFO) - Unlike regular SYN-ACK retransmit, we ignore error
	 * returned from rtx_syn_ack() to make it more persistent like
	 * regular retransmit because if the child socket has been accepted
	 * it's not good to give up too easily.
	 */
	inet_rtx_syn_ack(sk, req);
	req->num_timeout++;
	icsk->icsk_retransmits++;
	if (!tp->retrans_stamp)
		tp->retrans_stamp = tcp_time_stamp(tp);
	inet_csk_reset_xmit_timer(sk, ICSK_TIME_RETRANS,
			  TCP_TIMEOUT_INIT << req->num_timeout, TCP_RTO_MAX);
}