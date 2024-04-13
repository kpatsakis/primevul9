void sc_setup_user_mounts(struct sc_apparmor *apparmor, int snap_update_ns_fd,
			  const char *snap_name)
{
	debug("%s: %s", __FUNCTION__, snap_name);

	char profile_path[PATH_MAX];
	struct stat st;

	sc_must_snprintf(profile_path, sizeof(profile_path),
			 "/var/lib/snapd/mount/snap.%s.user-fstab", snap_name);
	if (stat(profile_path, &st) != 0) {
		// It is ok for the user fstab to not exist.
		return;
	}

	sc_make_slave_mount_ns();
	sc_call_snap_update_ns_as_user(snap_update_ns_fd, snap_name, apparmor);
}