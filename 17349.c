static inline void lock_rcu_walk(void)
{
	br_read_lock(&vfsmount_lock);
	rcu_read_lock();
}