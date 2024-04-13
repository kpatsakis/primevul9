static bool retransmits_timed_out(struct sock *sk,
				  unsigned int boundary,
				  unsigned int timeout)
{
	unsigned int start_ts;

	if (!inet_csk(sk)->icsk_retransmits)
		return false;

	start_ts = tcp_sk(sk)->retrans_stamp;
	if (likely(timeout == 0))
		timeout = tcp_model_timeout(sk, boundary, TCP_RTO_MIN);

	return (s32)(tcp_time_stamp(tcp_sk(sk)) - start_ts - timeout) >= 0;
}