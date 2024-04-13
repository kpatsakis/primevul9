static void rds_iw_ic_info(struct socket *sock, unsigned int len,
			   struct rds_info_iterator *iter,
			   struct rds_info_lengths *lens)
{
	rds_for_each_conn_info(sock, len, iter, lens,
				rds_iw_conn_info_visitor,
				sizeof(struct rds_info_rdma_connection));
}