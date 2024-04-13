int ipc_shm_get_limits(struct ipc_limits *lim)
{
	lim->shmmin = SHMMIN;

	if (access(_PATH_PROC_IPC_SHMALL, F_OK) == 0 &&
	    access(_PATH_PROC_IPC_SHMMAX, F_OK) == 0 &&
	    access(_PATH_PROC_IPC_SHMMNI, F_OK) == 0) {

		ul_path_read_u64(NULL, &lim->shmall, _PATH_PROC_IPC_SHMALL);
		ul_path_read_u64(NULL, &lim->shmmax, _PATH_PROC_IPC_SHMMAX);
		ul_path_read_u64(NULL, &lim->shmmni, _PATH_PROC_IPC_SHMMNI);

	} else {
		struct shminfo *shminfo;
		struct shmid_ds shmbuf;

		if (shmctl(0, IPC_INFO, &shmbuf) < 0)
			return 1;
		shminfo = (struct shminfo *) &shmbuf;
		lim->shmmni = shminfo->shmmni;
		lim->shmall = shminfo->shmall;
		lim->shmmax = shminfo->shmmax;
	}

	return 0;
}