void sc_ensure_shared_snap_mount(void)
{
	if (!is_mounted_with_shared_option("/")
	    && !is_mounted_with_shared_option(SNAP_MOUNT_DIR)) {
		// TODO: We could be more aggressive and refuse to function but since
		// we have no data on actual environments that happen to limp along in
		// this configuration let's not do that yet.  This code should be
		// removed once we have a measurement and feedback mechanism that lets
		// us decide based on measurable data.
		sc_do_mount(SNAP_MOUNT_DIR, SNAP_MOUNT_DIR, "none",
			    MS_BIND | MS_REC, 0);
		sc_do_mount("none", SNAP_MOUNT_DIR, NULL, MS_SHARED | MS_REC,
			    NULL);
	}
}