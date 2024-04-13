static void rds_conn_path_destroy(struct rds_conn_path *cp)
{
	struct rds_message *rm, *rtmp;

	if (!cp->cp_transport_data)
		return;

	/* make sure lingering queued work won't try to ref the conn */
	cancel_delayed_work_sync(&cp->cp_send_w);
	cancel_delayed_work_sync(&cp->cp_recv_w);

	rds_conn_path_drop(cp, true);
	flush_work(&cp->cp_down_w);

	/* tear down queued messages */
	list_for_each_entry_safe(rm, rtmp,
				 &cp->cp_send_queue,
				 m_conn_item) {
		list_del_init(&rm->m_conn_item);
		BUG_ON(!list_empty(&rm->m_sock_item));
		rds_message_put(rm);
	}
	if (cp->cp_xmit_rm)
		rds_message_put(cp->cp_xmit_rm);

	WARN_ON(delayed_work_pending(&cp->cp_send_w));
	WARN_ON(delayed_work_pending(&cp->cp_recv_w));
	WARN_ON(delayed_work_pending(&cp->cp_conn_w));
	WARN_ON(work_pending(&cp->cp_down_w));

	cp->cp_conn->c_trans->conn_free(cp->cp_transport_data);
}