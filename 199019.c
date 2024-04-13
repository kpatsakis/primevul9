static int sysvipc_shm_proc_show(struct seq_file *s, void *it)
{
	struct user_namespace *user_ns = seq_user_ns(s);
	struct shmid_kernel *shp = it;
	unsigned long rss = 0, swp = 0;

	shm_add_rss_swap(shp, &rss, &swp);

#if BITS_PER_LONG <= 32
#define SIZE_SPEC "%10lu"
#else
#define SIZE_SPEC "%21lu"
#endif

	seq_printf(s,
		   "%10d %10d  %4o " SIZE_SPEC " %5u %5u  "
		   "%5lu %5u %5u %5u %5u %10lu %10lu %10lu "
		   SIZE_SPEC " " SIZE_SPEC "\n",
		   shp->shm_perm.key,
		   shp->shm_perm.id,
		   shp->shm_perm.mode,
		   shp->shm_segsz,
		   shp->shm_cprid,
		   shp->shm_lprid,
		   shp->shm_nattch,
		   from_kuid_munged(user_ns, shp->shm_perm.uid),
		   from_kgid_munged(user_ns, shp->shm_perm.gid),
		   from_kuid_munged(user_ns, shp->shm_perm.cuid),
		   from_kgid_munged(user_ns, shp->shm_perm.cgid),
		   shp->shm_atim,
		   shp->shm_dtim,
		   shp->shm_ctim,
		   rss * PAGE_SIZE,
		   swp * PAGE_SIZE);

	return 0;
}