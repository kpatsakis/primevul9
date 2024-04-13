void rds_for_each_conn_info(struct socket *sock, unsigned int len,
			  struct rds_info_iterator *iter,
			  struct rds_info_lengths *lens,
			  int (*visitor)(struct rds_connection *, void *),
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

			/* XXX no c_lock usage.. */
			if (!visitor(conn, buffer))
				continue;

			/* We copy as much as we can fit in the buffer,
			 * but we count all items so that the caller
			 * can resize the buffer. */
			if (len >= item_len) {
				rds_info_copy(iter, buffer, item_len);
				len -= item_len;
			}
			lens->nr++;
		}
	}
	rcu_read_unlock();
}