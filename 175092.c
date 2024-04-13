void rds_check_all_paths(struct rds_connection *conn)
{
	int i = 0;

	do {
		rds_conn_path_connect_if_down(&conn->c_path[i]);
	} while (++i < conn->c_npaths);
}