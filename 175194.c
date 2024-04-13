	__releases(nfnl_queue_pernet(seq_file_net(s))->instances_lock)
{
	spin_unlock(&nfnl_queue_pernet(seq_file_net(s))->instances_lock);
}