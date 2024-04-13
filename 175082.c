static void rds_conn_message_info_cmn(struct socket *sock, unsigned int len,
				      struct rds_info_iterator *iter,
				      struct rds_info_lengths *lens,
				      int want_send, bool isv6)
{
	struct hlist_head *head;
	struct list_head *list;
	struct rds_connection *conn;
	struct rds_message *rm;
	unsigned int total = 0;
	unsigned long flags;
	size_t i;
	int j;

	if (isv6)
		len /= sizeof(struct rds6_info_message);
	else
		len /= sizeof(struct rds_info_message);

	rcu_read_lock();

	for (i = 0, head = rds_conn_hash; i < ARRAY_SIZE(rds_conn_hash);
	     i++, head++) {
		hlist_for_each_entry_rcu(conn, head, c_hash_node) {
			struct rds_conn_path *cp;
			int npaths;

			if (!isv6 && conn->c_isv6)
				continue;

			npaths = (conn->c_trans->t_mp_capable ?
				 RDS_MPATH_WORKERS : 1);

			for (j = 0; j < npaths; j++) {
				cp = &conn->c_path[j];
				if (want_send)
					list = &cp->cp_send_queue;
				else
					list = &cp->cp_retrans;

				spin_lock_irqsave(&cp->cp_lock, flags);

				/* XXX too lazy to maintain counts.. */
				list_for_each_entry(rm, list, m_conn_item) {
					total++;
					if (total <= len)
						__rds_inc_msg_cp(&rm->m_inc,
								 iter,
								 &conn->c_laddr,
								 &conn->c_faddr,
								 0, isv6);
				}

				spin_unlock_irqrestore(&cp->cp_lock, flags);
			}
		}
	}
	rcu_read_unlock();

	lens->nr = total;
	if (isv6)
		lens->each = sizeof(struct rds6_info_message);
	else
		lens->each = sizeof(struct rds_info_message);
}