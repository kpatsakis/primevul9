static int rds6_conn_info_visitor(struct rds_conn_path *cp, void *buffer)
{
	struct rds6_info_connection *cinfo6 = buffer;
	struct rds_connection *conn = cp->cp_conn;

	cinfo6->next_tx_seq = cp->cp_next_tx_seq;
	cinfo6->next_rx_seq = cp->cp_next_rx_seq;
	cinfo6->laddr = conn->c_laddr;
	cinfo6->faddr = conn->c_faddr;
	strncpy(cinfo6->transport, conn->c_trans->t_name,
		sizeof(cinfo6->transport));
	cinfo6->flags = 0;

	rds_conn_info_set(cinfo6->flags, test_bit(RDS_IN_XMIT, &cp->cp_flags),
			  SENDING);
	/* XXX Future: return the state rather than these funky bits */
	rds_conn_info_set(cinfo6->flags,
			  atomic_read(&cp->cp_state) == RDS_CONN_CONNECTING,
			  CONNECTING);
	rds_conn_info_set(cinfo6->flags,
			  atomic_read(&cp->cp_state) == RDS_CONN_UP,
			  CONNECTED);
	/* Just return 1 as there is no error case. This is a helper function
	 * for rds_walk_conn_path_info() and it wants a return value.
	 */
	return 1;
}