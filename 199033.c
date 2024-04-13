void shm_destroy_orphaned(struct ipc_namespace *ns)
{
	down_write(&shm_ids(ns).rwsem);
	if (shm_ids(ns).in_use)
		idr_for_each(&shm_ids(ns).ipcs_idr, &shm_try_destroy_orphaned, ns);
	up_write(&shm_ids(ns).rwsem);
}