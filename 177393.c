static int rds_release(struct socket *sock)
{
	struct sock *sk = sock->sk;
	struct rds_sock *rs;

	if (!sk)
		goto out;

	rs = rds_sk_to_rs(sk);

	sock_orphan(sk);
	/* Note - rds_clear_recv_queue grabs rs_recv_lock, so
	 * that ensures the recv path has completed messing
	 * with the socket. */
	rds_clear_recv_queue(rs);
	rds_cong_remove_socket(rs);

	rds_remove_bound(rs);

	rds_send_drop_to(rs, NULL);
	rds_rdma_drop_keys(rs);
	rds_notify_queue_get(rs, NULL);

	spin_lock_bh(&rds_sock_lock);
	list_del_init(&rs->rs_item);
	rds_sock_count--;
	spin_unlock_bh(&rds_sock_lock);

	rds_trans_put(rs->rs_transport);

	sock->sk = NULL;
	sock_put(sk);
out:
	return 0;
}