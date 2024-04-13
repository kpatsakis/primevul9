int ipc_sem_get_limits(struct ipc_limits *lim)
{
	FILE *f;
	int rc = 0;

	lim->semvmx = SEMVMX;

	f = fopen(_PATH_PROC_IPC_SEM, "r");
	if (f) {
		rc = fscanf(f, "%d\t%d\t%d\t%d",
		       &lim->semmsl, &lim->semmns, &lim->semopm, &lim->semmni);
		fclose(f);
	}

	if (rc != 4) {
		struct seminfo seminfo = { .semmni = 0 };
		union semun arg = { .array = (ushort *) &seminfo };

		if (semctl(0, 0, IPC_INFO, arg) < 0)
			return 1;
		lim->semmni = seminfo.semmni;
		lim->semmsl = seminfo.semmsl;
		lim->semmns = seminfo.semmns;
		lim->semopm = seminfo.semopm;
	}

	return 0;
}