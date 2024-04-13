	__acquires(atalk_sockets_lock)
{
	read_lock_bh(&atalk_sockets_lock);
	return seq_hlist_start_head(&atalk_sockets, *pos);
}