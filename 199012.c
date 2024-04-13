void shm_exit_ns(struct ipc_namespace *ns)
{
	free_ipcs(ns, &shm_ids(ns), do_shm_rmid);
	idr_destroy(&ns->ids[IPC_SHM_IDS].ipcs_idr);
}