bool lock_sock_fast(struct sock *sk)
{
	might_sleep();
	spin_lock_bh(&sk->sk_lock.slock);

	if (!sk->sk_lock.owned)
		/*
		 * Note : We must disable BH
		 */
		return false;

	__lock_sock(sk);
	sk->sk_lock.owned = 1;
	spin_unlock(&sk->sk_lock.slock);
	/*
	 * The sk_lock has mutex_lock() semantics here:
	 */
	mutex_acquire(&sk->sk_lock.dep_map, 0, 0, _RET_IP_);
	local_bh_enable();
	return true;
}