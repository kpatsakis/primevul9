static void netlink_undo_bind(int group, long unsigned int groups,
			      struct sock *sk)
{
	struct netlink_sock *nlk = nlk_sk(sk);
	int undo;

	if (!nlk->netlink_unbind)
		return;

	for (undo = 0; undo < group; undo++)
		if (test_bit(undo, &groups))
			nlk->netlink_unbind(sock_net(sk), undo + 1);
}