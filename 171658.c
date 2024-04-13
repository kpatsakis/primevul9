static void sock_def_write_space(struct sock *sk)
{
	struct socket_wq *wq;

	rcu_read_lock();

	/* Do not wake up a writer until he can make "significant"
	 * progress.  --DaveM
	 */
	if ((refcount_read(&sk->sk_wmem_alloc) << 1) <= sk->sk_sndbuf) {
		wq = rcu_dereference(sk->sk_wq);
		if (skwq_has_sleeper(wq))
			wake_up_interruptible_sync_poll(&wq->wait, POLLOUT |
						POLLWRNORM | POLLWRBAND);

		/* Should agree with poll, otherwise some programs break */
		if (sock_writeable(sk))
			sk_wake_async(sk, SOCK_WAKE_SPACE, POLL_OUT);
	}

	rcu_read_unlock();
}