void __init shm_init(void)
{
	ipc_init_proc_interface("sysvipc/shm",
#if BITS_PER_LONG <= 32
				"       key      shmid perms       size  cpid  lpid nattch   uid   gid  cuid  cgid      atime      dtime      ctime        rss       swap\n",
#else
				"       key      shmid perms                  size  cpid  lpid nattch   uid   gid  cuid  cgid      atime      dtime      ctime                   rss                  swap\n",
#endif
				IPC_SHM_IDS, sysvipc_shm_proc_show);
}