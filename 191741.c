static void netlink_update_socket_mc(struct netlink_sock *nlk,
				     unsigned int group,
				     int is_new)
{
	int old, new = !!is_new, subscriptions;

	old = test_bit(group - 1, nlk->groups);
	subscriptions = nlk->subscriptions - old + new;
	if (new)
		__set_bit(group - 1, nlk->groups);
	else
		__clear_bit(group - 1, nlk->groups);
	netlink_update_subscriptions(&nlk->sk, subscriptions);
	netlink_update_listeners(&nlk->sk);
}