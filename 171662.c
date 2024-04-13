int __sk_receive_skb(struct sock *sk, struct sk_buff *skb,
		     const int nested, unsigned int trim_cap, bool refcounted)
{
	int rc = NET_RX_SUCCESS;

	if (sk_filter_trim_cap(sk, skb, trim_cap))
		goto discard_and_relse;

	skb->dev = NULL;

	if (sk_rcvqueues_full(sk, sk->sk_rcvbuf)) {
		atomic_inc(&sk->sk_drops);
		goto discard_and_relse;
	}
	if (nested)
		bh_lock_sock_nested(sk);
	else
		bh_lock_sock(sk);
	if (!sock_owned_by_user(sk)) {
		/*
		 * trylock + unlock semantics:
		 */
		mutex_acquire(&sk->sk_lock.dep_map, 0, 1, _RET_IP_);

		rc = sk_backlog_rcv(sk, skb);

		mutex_release(&sk->sk_lock.dep_map, 1, _RET_IP_);
	} else if (sk_add_backlog(sk, skb, sk->sk_rcvbuf)) {
		bh_unlock_sock(sk);
		atomic_inc(&sk->sk_drops);
		goto discard_and_relse;
	}

	bh_unlock_sock(sk);
out:
	if (refcounted)
		sock_put(sk);
	return rc;
discard_and_relse:
	kfree_skb(skb);
	goto out;
}