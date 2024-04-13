void rds_conn_shutdown(struct rds_conn_path *cp)
{
	struct rds_connection *conn = cp->cp_conn;

	/* shut it down unless it's down already */
	if (!rds_conn_path_transition(cp, RDS_CONN_DOWN, RDS_CONN_DOWN)) {
		/*
		 * Quiesce the connection mgmt handlers before we start tearing
		 * things down. We don't hold the mutex for the entire
		 * duration of the shutdown operation, else we may be
		 * deadlocking with the CM handler. Instead, the CM event
		 * handler is supposed to check for state DISCONNECTING
		 */
		mutex_lock(&cp->cp_cm_lock);
		if (!rds_conn_path_transition(cp, RDS_CONN_UP,
					      RDS_CONN_DISCONNECTING) &&
		    !rds_conn_path_transition(cp, RDS_CONN_ERROR,
					      RDS_CONN_DISCONNECTING)) {
			rds_conn_path_error(cp,
					    "shutdown called in state %d\n",
					    atomic_read(&cp->cp_state));
			mutex_unlock(&cp->cp_cm_lock);
			return;
		}
		mutex_unlock(&cp->cp_cm_lock);

		wait_event(cp->cp_waitq,
			   !test_bit(RDS_IN_XMIT, &cp->cp_flags));
		wait_event(cp->cp_waitq,
			   !test_bit(RDS_RECV_REFILL, &cp->cp_flags));

		conn->c_trans->conn_path_shutdown(cp);
		rds_conn_path_reset(cp);

		if (!rds_conn_path_transition(cp, RDS_CONN_DISCONNECTING,
					      RDS_CONN_DOWN) &&
		    !rds_conn_path_transition(cp, RDS_CONN_ERROR,
					      RDS_CONN_DOWN)) {
			/* This can happen - eg when we're in the middle of tearing
			 * down the connection, and someone unloads the rds module.
			 * Quite reproducible with loopback connections.
			 * Mostly harmless.
			 *
			 * Note that this also happens with rds-tcp because
			 * we could have triggered rds_conn_path_drop in irq
			 * mode from rds_tcp_state change on the receipt of
			 * a FIN, thus we need to recheck for RDS_CONN_ERROR
			 * here.
			 */
			rds_conn_path_error(cp, "%s: failed to transition "
					    "to state DOWN, current state "
					    "is %d\n", __func__,
					    atomic_read(&cp->cp_state));
			return;
		}
	}

	/* Then reconnect if it's still live.
	 * The passive side of an IB loopback connection is never added
	 * to the conn hash, so we never trigger a reconnect on this
	 * conn - the reconnect is always triggered by the active peer. */
	cancel_delayed_work_sync(&cp->cp_conn_w);
	rcu_read_lock();
	if (!hlist_unhashed(&conn->c_hash_node)) {
		rcu_read_unlock();
		rds_queue_reconnect(cp);
	} else {
		rcu_read_unlock();
	}
}