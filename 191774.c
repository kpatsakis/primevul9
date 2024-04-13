netlink_update_subscriptions(struct sock *sk, unsigned int subscriptions)
{
	struct netlink_sock *nlk = nlk_sk(sk);

	if (nlk->subscriptions && !subscriptions)
		__sk_del_bind_node(sk);
	else if (!nlk->subscriptions && subscriptions)
		sk_add_bind_node(sk, &nl_table[sk->sk_protocol].mc_list);
	nlk->subscriptions = subscriptions;
}