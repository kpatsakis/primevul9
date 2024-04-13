	__acquires(rcu_bh)
{
	rcu_read_lock_bh();
	return if6_get_first(seq, *pos);
}