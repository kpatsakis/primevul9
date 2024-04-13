static long sock_wait_for_wmem(struct sock *sk, long timeo)
{
	DEFINE_WAIT(wait);

	sk_clear_bit(SOCKWQ_ASYNC_NOSPACE, sk);
	for (;;) {
		if (!timeo)
			break;
		if (signal_pending(current))
			break;
		set_bit(SOCK_NOSPACE, &sk->sk_socket->flags);
		prepare_to_wait(sk_sleep(sk), &wait, TASK_INTERRUPTIBLE);
		if (refcount_read(&sk->sk_wmem_alloc) < sk->sk_sndbuf)
			break;
		if (sk->sk_shutdown & SEND_SHUTDOWN)
			break;
		if (sk->sk_err)
			break;
		timeo = schedule_timeout(timeo);
	}
	finish_wait(sk_sleep(sk), &wait);
	return timeo;
}