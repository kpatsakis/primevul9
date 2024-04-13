static inline void shm_lock_by_ptr(struct shmid_kernel *ipcp)
{
	rcu_read_lock();
	ipc_lock_object(&ipcp->shm_perm);
}