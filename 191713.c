int netlink_attachskb(struct sock *sk, struct sk_buff *skb,
		      long *timeo, struct sock *ssk)
{
	struct netlink_sock *nlk;

	nlk = nlk_sk(sk);

	if ((atomic_read(&sk->sk_rmem_alloc) > sk->sk_rcvbuf ||
	     test_bit(NETLINK_S_CONGESTED, &nlk->state))) {
		DECLARE_WAITQUEUE(wait, current);
		if (!*timeo) {
			if (!ssk || netlink_is_kernel(ssk))
				netlink_overrun(sk);
			sock_put(sk);
			kfree_skb(skb);
			return -EAGAIN;
		}

		__set_current_state(TASK_INTERRUPTIBLE);
		add_wait_queue(&nlk->wait, &wait);

		if ((atomic_read(&sk->sk_rmem_alloc) > sk->sk_rcvbuf ||
		     test_bit(NETLINK_S_CONGESTED, &nlk->state)) &&
		    !sock_flag(sk, SOCK_DEAD))
			*timeo = schedule_timeout(*timeo);

		__set_current_state(TASK_RUNNING);
		remove_wait_queue(&nlk->wait, &wait);
		sock_put(sk);

		if (signal_pending(current)) {
			kfree_skb(skb);
			return sock_intr_errno(*timeo);
		}
		return 1;
	}
	netlink_skb_set_owner_r(skb, sk);
	return 0;
}