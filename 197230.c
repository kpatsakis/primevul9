do_mount (struct mountargs *args, int *special, int *status) {
	int ret;
	if (check_special_mountprog(args->spec, args->node, args->type,
	                            args->flags, args->data, status)) {
		*special = 1;
		ret = 0;
	} else {
		ret = do_mount_syscall(args);
	}
	if (ret == 0)
		mountcount++;
	return ret;
}