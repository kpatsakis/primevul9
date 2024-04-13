void __init proc_root_init(void)
{
	int err;

	proc_init_inodecache();
	err = register_filesystem(&proc_fs_type);
	if (err)
		return;

	proc_self_init();
	proc_thread_self_init();
	proc_symlink("mounts", NULL, "self/mounts");

	proc_net_init();

#ifdef CONFIG_SYSVIPC
	proc_mkdir("sysvipc", NULL);
#endif
	proc_mkdir("fs", NULL);
	proc_mkdir("driver", NULL);
	proc_create_mount_point("fs/nfsd"); /* somewhere for the nfsd filesystem to be mounted */
#if defined(CONFIG_SUN_OPENPROMFS) || defined(CONFIG_SUN_OPENPROMFS_MODULE)
	/* just give it a mountpoint */
	proc_create_mount_point("openprom");
#endif
	proc_tty_init();
	proc_mkdir("bus", NULL);
	proc_sys_init();
}