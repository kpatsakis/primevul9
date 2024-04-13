static struct rds_connection *rds_conn_lookup(struct net *net,
					      struct hlist_head *head,
					      const struct in6_addr *laddr,
					      const struct in6_addr *faddr,
					      struct rds_transport *trans,
					      u8 tos, int dev_if)
{
	struct rds_connection *conn, *ret = NULL;

	hlist_for_each_entry_rcu(conn, head, c_hash_node) {
		if (ipv6_addr_equal(&conn->c_faddr, faddr) &&
		    ipv6_addr_equal(&conn->c_laddr, laddr) &&
		    conn->c_trans == trans &&
		    conn->c_tos == tos &&
		    net == rds_conn_net(conn) &&
		    conn->c_dev_if == dev_if) {
			ret = conn;
			break;
		}
	}
	rdsdebug("returning conn %p for %pI6c -> %pI6c\n", ret,
		 laddr, faddr);
	return ret;
}