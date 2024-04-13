static void sc_make_slave_mount_ns(void)
{
	if (unshare(CLONE_NEWNS) < 0) {
		die("can not unshare mount namespace");
	}
	// In our new mount namespace, recursively change all mounts
	// to slave mode, so we see changes from the parent namespace
	// but don't propagate our own changes.
	sc_do_mount("none", "/", NULL, MS_REC | MS_SLAVE, NULL);
}