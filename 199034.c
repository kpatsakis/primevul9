static inline int shm_security(struct kern_ipc_perm *ipcp, int shmflg)
{
	struct shmid_kernel *shp;

	shp = container_of(ipcp, struct shmid_kernel, shm_perm);
	return security_shm_associate(shp, shmflg);
}