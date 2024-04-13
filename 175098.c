void rds_conn_path_connect_if_down(struct rds_conn_path *cp)
{
	rcu_read_lock();
	if (rds_destroy_pending(cp->cp_conn)) {
		rcu_read_unlock();
		return;
	}
	if (rds_conn_path_state(cp) == RDS_CONN_DOWN &&
	    !test_and_set_bit(RDS_RECONNECT_PENDING, &cp->cp_flags))
		queue_delayed_work(rds_wq, &cp->cp_conn_w, 0);
	rcu_read_unlock();
}