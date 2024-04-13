void af_alg_wmem_wakeup(struct sock *sk)
{
	struct socket_wq *wq;

	if (!af_alg_writable(sk))
		return;

	rcu_read_lock();
	wq = rcu_dereference(sk->sk_wq);
	if (skwq_has_sleeper(wq))
		wake_up_interruptible_sync_poll(&wq->wait, EPOLLIN |
							   EPOLLRDNORM |
							   EPOLLRDBAND);
	sk_wake_async(sk, SOCK_WAKE_WAITD, POLL_IN);
	rcu_read_unlock();
}