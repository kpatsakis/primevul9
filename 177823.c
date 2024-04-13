static void setup_private_pts(void)
{
	// See https://www.kernel.org/doc/Documentation/filesystems/devpts.txt
	//
	// Ubuntu by default uses devpts 'single-instance' mode where
	// /dev/pts/ptmx is mounted with ptmxmode=0000. We don't want to change
	// the startup scripts though, so we follow the instructions in point
	// '4' of 'User-space changes' in the above doc. In other words, after
	// unshare(CLONE_NEWNS), we mount devpts with -o
	// newinstance,ptmxmode=0666 and then bind mount /dev/pts/ptmx onto
	// /dev/ptmx

	struct stat st;

	// Make sure /dev/pts/ptmx exists, otherwise we are in legacy mode
	// which doesn't provide the isolation we require.
	if (stat("/dev/pts/ptmx", &st) != 0) {
		die("cannot stat /dev/pts/ptmx");
	}
	// Make sure /dev/ptmx exists so we can bind mount over it
	if (stat("/dev/ptmx", &st) != 0) {
		die("cannot stat /dev/ptmx");
	}
	// Since multi-instance, use ptmxmode=0666. The other options are
	// copied from /etc/default/devpts
	sc_do_mount("devpts", "/dev/pts", "devpts", MS_MGC_VAL,
		    "newinstance,ptmxmode=0666,mode=0620,gid=5");
	sc_do_mount("/dev/pts/ptmx", "/dev/ptmx", "none", MS_BIND, 0);
}