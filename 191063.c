void dccp_write_space(struct sock *sk)
{
	read_lock(&sk->sk_callback_lock);

	if (sk->sk_sleep && waitqueue_active(sk->sk_sleep))
		wake_up_interruptible(sk->sk_sleep);
	/* Should agree with poll, otherwise some programs break */
	if (sock_writeable(sk))
		sk_wake_async(sk, SOCK_WAKE_SPACE, POLL_OUT);

	read_unlock(&sk->sk_callback_lock);
}