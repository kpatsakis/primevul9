static void netlink_sock_destruct(struct sock *sk)
{
	struct netlink_sock *nlk = nlk_sk(sk);

	if (nlk->cb_running) {
		if (nlk->cb.done)
			nlk->cb.done(&nlk->cb);
		module_put(nlk->cb.module);
		kfree_skb(nlk->cb.skb);
	}

	skb_queue_purge(&sk->sk_receive_queue);

	if (!sock_flag(sk, SOCK_DEAD)) {
		printk(KERN_ERR "Freeing alive netlink socket %p\n", sk);
		return;
	}

	WARN_ON(atomic_read(&sk->sk_rmem_alloc));
	WARN_ON(refcount_read(&sk->sk_wmem_alloc));
	WARN_ON(nlk_sk(sk)->groups);
}