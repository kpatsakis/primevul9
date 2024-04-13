static void sock_def_error_report(struct sock *sk)
{
	struct socket_wq *wq;

	rcu_read_lock();
	wq = rcu_dereference(sk->sk_wq);
	if (skwq_has_sleeper(wq))
		wake_up_interruptible_poll(&wq->wait, POLLERR);
	sk_wake_async(sk, SOCK_WAKE_IO, POLL_ERR);
	rcu_read_unlock();
}