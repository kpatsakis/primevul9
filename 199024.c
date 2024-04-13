static bool shm_may_destroy(struct ipc_namespace *ns, struct shmid_kernel *shp)
{
	return (shp->shm_nattch == 0) &&
	       (ns->shm_rmid_forced ||
		(shp->shm_perm.mode & SHM_DEST));
}