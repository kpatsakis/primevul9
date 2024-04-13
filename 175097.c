static int rds_conn_info_visitor(struct rds_conn_path *cp, void *buffer)
{
	struct rds_info_connection *cinfo = buffer;
	struct rds_connection *conn = cp->cp_conn;

	if (conn->c_isv6)
		return 0;

	cinfo->next_tx_seq = cp->cp_next_tx_seq;
	cinfo->next_rx_seq = cp->cp_next_rx_seq;
	cinfo->laddr = conn->c_laddr.s6_addr32[3];
	cinfo->faddr = conn->c_faddr.s6_addr32[3];
	cinfo->tos = conn->c_tos;
	strncpy(cinfo->transport, conn->c_trans->t_name,
		sizeof(cinfo->transport));
	cinfo->flags = 0;

	rds_conn_info_set(cinfo->flags, test_bit(RDS_IN_XMIT, &cp->cp_flags),
			  SENDING);
	/* XXX Future: return the state rather than these funky bits */
	rds_conn_info_set(cinfo->flags,
			  atomic_read(&cp->cp_state) == RDS_CONN_CONNECTING,
			  CONNECTING);
	rds_conn_info_set(cinfo->flags,
			  atomic_read(&cp->cp_state) == RDS_CONN_UP,
			  CONNECTED);
	return 1;
}