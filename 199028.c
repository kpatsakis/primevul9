static inline int shm_more_checks(struct kern_ipc_perm *ipcp,
				struct ipc_params *params)
{
	struct shmid_kernel *shp;

	shp = container_of(ipcp, struct shmid_kernel, shm_perm);
	if (shp->shm_segsz < params->u.size)
		return -EINVAL;

	return 0;
}