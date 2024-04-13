void rds_conn_destroy(struct rds_connection *conn)
{
	unsigned long flags;
	int i;
	struct rds_conn_path *cp;
	int npaths = (conn->c_trans->t_mp_capable ? RDS_MPATH_WORKERS : 1);

	rdsdebug("freeing conn %p for %pI4 -> "
		 "%pI4\n", conn, &conn->c_laddr,
		 &conn->c_faddr);

	/* Ensure conn will not be scheduled for reconnect */
	spin_lock_irq(&rds_conn_lock);
	hlist_del_init_rcu(&conn->c_hash_node);
	spin_unlock_irq(&rds_conn_lock);
	synchronize_rcu();

	/* shut the connection down */
	for (i = 0; i < npaths; i++) {
		cp = &conn->c_path[i];
		rds_conn_path_destroy(cp);
		BUG_ON(!list_empty(&cp->cp_retrans));
	}

	/*
	 * The congestion maps aren't freed up here.  They're
	 * freed by rds_cong_exit() after all the connections
	 * have been freed.
	 */
	rds_cong_remove_conn(conn);

	kfree(conn->c_path);
	kmem_cache_free(rds_conn_slab, conn);

	spin_lock_irqsave(&rds_conn_lock, flags);
	rds_conn_count--;
	spin_unlock_irqrestore(&rds_conn_lock, flags);
}