static int shm_try_destroy_orphaned(int id, void *p, void *data)
{
	struct ipc_namespace *ns = data;
	struct kern_ipc_perm *ipcp = p;
	struct shmid_kernel *shp = container_of(ipcp, struct shmid_kernel, shm_perm);

	/*
	 * We want to destroy segments without users and with already
	 * exit'ed originating process.
	 *
	 * As shp->* are changed under rwsem, it's safe to skip shp locking.
	 */
	if (shp->shm_creator != NULL)
		return 0;

	if (shm_may_destroy(ns, shp)) {
		shm_lock_by_ptr(shp);
		shm_destroy(ns, shp);
	}
	return 0;
}