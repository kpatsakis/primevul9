static void shm_get_stat(struct ipc_namespace *ns, unsigned long *rss,
		unsigned long *swp)
{
	int next_id;
	int total, in_use;

	*rss = 0;
	*swp = 0;

	in_use = shm_ids(ns).in_use;

	for (total = 0, next_id = 0; total < in_use; next_id++) {
		struct kern_ipc_perm *ipc;
		struct shmid_kernel *shp;

		ipc = idr_find(&shm_ids(ns).ipcs_idr, next_id);
		if (ipc == NULL)
			continue;
		shp = container_of(ipc, struct shmid_kernel, shm_perm);

		shm_add_rss_swap(shp, rss, swp);

		total++;
	}
}