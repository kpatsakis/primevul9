bool sk_busy_loop_end(void *p, unsigned long start_time)
{
	struct sock *sk = p;

	return !skb_queue_empty(&sk->sk_receive_queue) ||
	       sk_busy_loop_timeout(sk, start_time);
}