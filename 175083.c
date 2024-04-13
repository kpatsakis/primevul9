static void rds_walk_conn_path_info(struct socket *sock, unsigned int len,
				    struct rds_info_iterator *iter,
				    struct rds_info_lengths *lens,
				    int (*visitor)(struct rds_conn_path *, void *),
				    u64 *buffer,
				    size_t item_len)
{
	struct hlist_head *head;
	struct rds_connection *conn;
	size_t i;

	rcu_read_lock();

	lens->nr = 0;
	lens->each = item_len;

	for (i = 0, head = rds_conn_hash; i < ARRAY_SIZE(rds_conn_hash);
	     i++, head++) {
		hlist_for_each_entry_rcu(conn, head, c_hash_node) {
			struct rds_conn_path *cp;

			/* XXX We only copy the information from the first
			 * path for now.  The problem is that if there are
			 * more than one underlying paths, we cannot report
			 * information of all of them using the existing
			 * API.  For example, there is only one next_tx_seq,
			 * which path's next_tx_seq should we report?  It is
			 * a bug in the design of MPRDS.
			 */
			cp = conn->c_path;

			/* XXX no cp_lock usage.. */
			if (!visitor(cp, buffer))
				continue;

			/* We copy as much as we can fit in the buffer,
			 * but we count all items so that the caller
			 * can resize the buffer.
			 */
			if (len >= item_len) {
				rds_info_copy(iter, buffer, item_len);
				len -= item_len;
			}
			lens->nr++;
		}
	}
	rcu_read_unlock();
}