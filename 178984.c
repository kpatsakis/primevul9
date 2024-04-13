	__releases(atalk_sockets_lock)
{
	read_unlock_bh(&atalk_sockets_lock);
}