static void rds6_conn_info(struct socket *sock, unsigned int len,
			   struct rds_info_iterator *iter,
			   struct rds_info_lengths *lens)
{
	u64 buffer[(sizeof(struct rds6_info_connection) + 7) / 8];

	rds_walk_conn_path_info(sock, len, iter, lens,
				rds6_conn_info_visitor,
				buffer,
				sizeof(struct rds6_info_connection));
}