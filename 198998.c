SYSCALL_DEFINE3(shmget, key_t, key, size_t, size, int, shmflg)
{
	struct ipc_namespace *ns;
	static const struct ipc_ops shm_ops = {
		.getnew = newseg,
		.associate = shm_security,
		.more_checks = shm_more_checks,
	};
	struct ipc_params shm_params;

	ns = current->nsproxy->ipc_ns;

	shm_params.key = key;
	shm_params.flg = shmflg;
	shm_params.u.size = size;

	return ipcget(ns, &shm_ids(ns), &shm_ops, &shm_params);
}