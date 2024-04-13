	__acquires(rcu)
{
	rcu_read_lock();
	return *pos ? igmp_mcf_get_idx(seq, *pos - 1) : SEQ_START_TOKEN;
}