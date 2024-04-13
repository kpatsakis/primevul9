	__acquires(rcu)
{
	rcu_read_lock();
	return *pos ? igmp_mc_get_idx(seq, *pos - 1) : SEQ_START_TOKEN;
}