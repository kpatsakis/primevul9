static void do_shm_rmid(struct ipc_namespace *ns, struct kern_ipc_perm *ipcp)
{
	struct shmid_kernel *shp;

	shp = container_of(ipcp, struct shmid_kernel, shm_perm);

	if (shp->shm_nattch) {
		shp->shm_perm.mode |= SHM_DEST;
		/* Do not find it any more */
		shp->shm_perm.key = IPC_PRIVATE;
		shm_unlock(shp);
	} else
		shm_destroy(ns, shp);
}