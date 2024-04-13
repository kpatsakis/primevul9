static void rds6_conn_message_info(struct socket *sock, unsigned int len,
				   struct rds_info_iterator *iter,
				   struct rds_info_lengths *lens,
				   int want_send)
{
	rds_conn_message_info_cmn(sock, len, iter, lens, want_send, true);
}