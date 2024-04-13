	__acquires(atalk_interfaces_lock)
{
	loff_t l = *pos;

	read_lock_bh(&atalk_interfaces_lock);
	return l ? atalk_get_interface_idx(--l) : SEQ_START_TOKEN;
}