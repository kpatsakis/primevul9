void shm_init_ns(struct ipc_namespace *ns)
{
	ns->shm_ctlmax = SHMMAX;
	ns->shm_ctlall = SHMALL;
	ns->shm_ctlmni = SHMMNI;
	ns->shm_rmid_forced = 0;
	ns->shm_tot = 0;
	ipc_init_ids(&shm_ids(ns));
}