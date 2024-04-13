static void netlink_rcv_wake(struct sock *sk)
{
	struct netlink_sock *nlk = nlk_sk(sk);

	if (skb_queue_empty(&sk->sk_receive_queue))
		clear_bit(NETLINK_S_CONGESTED, &nlk->state);
	if (!test_bit(NETLINK_S_CONGESTED, &nlk->state))
		wake_up_interruptible(&nlk->wait);
}