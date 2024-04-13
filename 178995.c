	__acquires(atalk_routes_lock)
{
	loff_t l = *pos;

	read_lock_bh(&atalk_routes_lock);
	return l ? atalk_get_route_idx(--l) : SEQ_START_TOKEN;
}