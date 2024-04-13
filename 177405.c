static int __rds_create(struct socket *sock, struct sock *sk, int protocol)
{
	struct rds_sock *rs;

	sock_init_data(sock, sk);
	sock->ops		= &rds_proto_ops;
	sk->sk_protocol		= protocol;
	sk->sk_destruct		= rds_sock_destruct;

	rs = rds_sk_to_rs(sk);
	spin_lock_init(&rs->rs_lock);
	rwlock_init(&rs->rs_recv_lock);
	INIT_LIST_HEAD(&rs->rs_send_queue);
	INIT_LIST_HEAD(&rs->rs_recv_queue);
	INIT_LIST_HEAD(&rs->rs_notify_queue);
	INIT_LIST_HEAD(&rs->rs_cong_list);
	spin_lock_init(&rs->rs_rdma_lock);
	rs->rs_rdma_keys = RB_ROOT;
	rs->rs_rx_traces = 0;

	spin_lock_bh(&rds_sock_lock);
	list_add_tail(&rs->rs_item, &rds_sock_list);
	rds_sock_count++;
	spin_unlock_bh(&rds_sock_lock);

	return 0;
}