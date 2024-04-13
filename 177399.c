static void rds_sock_inc_info(struct socket *sock, unsigned int len,
			      struct rds_info_iterator *iter,
			      struct rds_info_lengths *lens)
{
	struct rds_sock *rs;
	struct rds_incoming *inc;
	unsigned int total = 0;

	len /= sizeof(struct rds_info_message);

	spin_lock_bh(&rds_sock_lock);

	list_for_each_entry(rs, &rds_sock_list, rs_item) {
		read_lock(&rs->rs_recv_lock);

		/* XXX too lazy to maintain counts.. */
		list_for_each_entry(inc, &rs->rs_recv_queue, i_item) {
			total++;
			if (total <= len)
				rds_inc_info_copy(inc, iter, inc->i_saddr,
						  rs->rs_bound_addr, 1);
		}

		read_unlock(&rs->rs_recv_lock);
	}

	spin_unlock_bh(&rds_sock_lock);

	lens->nr = total;
	lens->each = sizeof(struct rds_info_message);
}