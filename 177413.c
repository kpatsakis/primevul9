static void rds_sock_info(struct socket *sock, unsigned int len,
			  struct rds_info_iterator *iter,
			  struct rds_info_lengths *lens)
{
	struct rds_info_socket sinfo;
	struct rds_sock *rs;

	len /= sizeof(struct rds_info_socket);

	spin_lock_bh(&rds_sock_lock);

	if (len < rds_sock_count)
		goto out;

	list_for_each_entry(rs, &rds_sock_list, rs_item) {
		sinfo.sndbuf = rds_sk_sndbuf(rs);
		sinfo.rcvbuf = rds_sk_rcvbuf(rs);
		sinfo.bound_addr = rs->rs_bound_addr;
		sinfo.connected_addr = rs->rs_conn_addr;
		sinfo.bound_port = rs->rs_bound_port;
		sinfo.connected_port = rs->rs_conn_port;
		sinfo.inum = sock_i_ino(rds_rs_to_sk(rs));

		rds_info_copy(iter, &sinfo, sizeof(sinfo));
	}

out:
	lens->nr = rds_sock_count;
	lens->each = sizeof(struct rds_info_socket);

	spin_unlock_bh(&rds_sock_lock);
}