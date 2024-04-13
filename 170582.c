	__acquires(aarp_lock)
{
	struct aarp_iter_state *iter = seq->private;

	read_lock_bh(&aarp_lock);
	iter->table     = resolved;
	iter->bucket    = 0;

	return *pos ? iter_next(iter, pos) : SEQ_START_TOKEN;
}