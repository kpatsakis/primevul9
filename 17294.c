static inline void unlock_rcu_walk(void)
{
	rcu_read_unlock();
	br_read_unlock(&vfsmount_lock);
}