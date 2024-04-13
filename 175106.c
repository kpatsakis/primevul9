static void rds_conn_path_reset(struct rds_conn_path *cp)
{
	struct rds_connection *conn = cp->cp_conn;

	rdsdebug("connection %pI6c to %pI6c reset\n",
		 &conn->c_laddr, &conn->c_faddr);

	rds_stats_inc(s_conn_reset);
	rds_send_path_reset(cp);
	cp->cp_flags = 0;

	/* Do not clear next_rx_seq here, else we cannot distinguish
	 * retransmitted packets from new packets, and will hand all
	 * of them to the application. That is not consistent with the
	 * reliability guarantees of RDS. */
}