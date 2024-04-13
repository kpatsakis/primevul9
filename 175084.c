void rds_conn_path_drop(struct rds_conn_path *cp, bool destroy)
{
	atomic_set(&cp->cp_state, RDS_CONN_ERROR);

	rcu_read_lock();
	if (!destroy && rds_destroy_pending(cp->cp_conn)) {
		rcu_read_unlock();
		return;
	}
	queue_work(rds_wq, &cp->cp_down_w);
	rcu_read_unlock();
}