void af_alg_data_wakeup(struct sock *sk)
{
	struct alg_sock *ask = alg_sk(sk);
	struct af_alg_ctx *ctx = ask->private;
	struct socket_wq *wq;

	if (!ctx->used)
		return;

	rcu_read_lock();
	wq = rcu_dereference(sk->sk_wq);
	if (skwq_has_sleeper(wq))
		wake_up_interruptible_sync_poll(&wq->wait, EPOLLOUT |
							   EPOLLRDNORM |
							   EPOLLRDBAND);
	sk_wake_async(sk, SOCK_WAKE_SPACE, POLL_OUT);
	rcu_read_unlock();
}