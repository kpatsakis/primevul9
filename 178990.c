	__releases(atalk_interfaces_lock)
{
	read_unlock_bh(&atalk_interfaces_lock);
}