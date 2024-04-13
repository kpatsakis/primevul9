static void __rds_conn_path_init(struct rds_connection *conn,
				 struct rds_conn_path *cp, bool is_outgoing)
{
	spin_lock_init(&cp->cp_lock);
	cp->cp_next_tx_seq = 1;
	init_waitqueue_head(&cp->cp_waitq);
	INIT_LIST_HEAD(&cp->cp_send_queue);
	INIT_LIST_HEAD(&cp->cp_retrans);

	cp->cp_conn = conn;
	atomic_set(&cp->cp_state, RDS_CONN_DOWN);
	cp->cp_send_gen = 0;
	cp->cp_reconnect_jiffies = 0;
	cp->cp_conn->c_proposed_version = RDS_PROTOCOL_VERSION;
	INIT_DELAYED_WORK(&cp->cp_send_w, rds_send_worker);
	INIT_DELAYED_WORK(&cp->cp_recv_w, rds_recv_worker);
	INIT_DELAYED_WORK(&cp->cp_conn_w, rds_connect_worker);
	INIT_WORK(&cp->cp_down_w, rds_shutdown_worker);
	mutex_init(&cp->cp_cm_lock);
	cp->cp_flags = 0;
}