	__acquires(nfnl_queue_pernet(seq_file_net(s))->instances_lock)
{
	spin_lock(&nfnl_queue_pernet(seq_file_net(s))->instances_lock);
	return get_idx(s, *pos);
}