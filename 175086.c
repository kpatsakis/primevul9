static void rds6_conn_message_info_retrans(struct socket *sock,
					   unsigned int len,
					   struct rds_info_iterator *iter,
					   struct rds_info_lengths *lens)
{
	rds6_conn_message_info(sock, len, iter, lens, 0);
}