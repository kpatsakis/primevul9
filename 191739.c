static void deferred_put_nlk_sk(struct rcu_head *head)
{
	struct netlink_sock *nlk = container_of(head, struct netlink_sock, rcu);
	struct sock *sk = &nlk->sk;

	kfree(nlk->groups);
	nlk->groups = NULL;

	if (!refcount_dec_and_test(&sk->sk_refcnt))
		return;

	if (nlk->cb_running && nlk->cb.done) {
		INIT_WORK(&nlk->work, netlink_sock_destruct_work);
		schedule_work(&nlk->work);
		return;
	}

	sk_free(sk);
}