static void netlink_sock_destruct_work(struct work_struct *work)
{
	struct netlink_sock *nlk = container_of(work, struct netlink_sock,
						work);

	sk_free(&nlk->sk);
}