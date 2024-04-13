void rds_conn_drop(struct rds_connection *conn)
{
	WARN_ON(conn->c_trans->t_mp_capable);
	rds_conn_path_drop(&conn->c_path[0], false);
}