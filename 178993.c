static void *atalk_seq_socket_next(struct seq_file *seq, void *v, loff_t *pos)
{
	return seq_hlist_next(v, &atalk_sockets, pos);
}