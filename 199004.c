static void shm_rcu_free(struct rcu_head *head)
{
	struct ipc_rcu *p = container_of(head, struct ipc_rcu, rcu);
	struct shmid_kernel *shp = ipc_rcu_to_struct(p);

	security_shm_free(shp);
	ipc_rcu_free(head);
}