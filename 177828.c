void sc_populate_mount_ns(struct sc_apparmor *apparmor, int snap_update_ns_fd,
			  const char *base_snap_name, const char *snap_name)
{
	// Get the current working directory before we start fiddling with
	// mounts and possibly pivot_root.  At the end of the whole process, we
	// will try to re-locate to the same directory (if possible).
	char *vanilla_cwd SC_CLEANUP(sc_cleanup_string) = NULL;
	vanilla_cwd = get_current_dir_name();
	if (vanilla_cwd == NULL) {
		die("cannot get the current working directory");
	}
	// Classify the current distribution, as claimed by /etc/os-release.
	sc_distro distro = sc_classify_distro();
	// Check which mode we should run in, normal or legacy.
	if (sc_should_use_normal_mode(distro, base_snap_name)) {
		// In normal mode we use the base snap as / and set up several bind mounts.
		const struct sc_mount mounts[] = {
			{"/dev"},	// because it contains devices on host OS
			{"/etc"},	// because that's where /etc/resolv.conf lives, perhaps a bad idea
			{"/home"},	// to support /home/*/snap and home interface
			{"/root"},	// because that is $HOME for services
			{"/proc"},	// fundamental filesystem
			{"/sys"},	// fundamental filesystem
			{"/tmp"},	// to get writable tmp
			{"/var/snap"},	// to get access to global snap data
			{"/var/lib/snapd"},	// to get access to snapd state and seccomp profiles
			{"/var/tmp"},	// to get access to the other temporary directory
			{"/run"},	// to get /run with sockets and what not
			{"/lib/modules",.is_optional = true},	// access to the modules of the running kernel
			{"/usr/src"},	// FIXME: move to SecurityMounts in system-trace interface
			{"/var/log"},	// FIXME: move to SecurityMounts in log-observe interface
#ifdef MERGED_USR
			{"/run/media", true, "/media"},	// access to the users removable devices
#else
			{"/media", true},	// access to the users removable devices
#endif				// MERGED_USR
			{"/run/netns", true},	// access to the 'ip netns' network namespaces
			// The /mnt directory is optional in base snaps to ensure backwards
			// compatibility with the first version of base snaps that was
			// released.
			{"/mnt",.is_optional = true},	// to support the removable-media interface
			{"/var/lib/extrausers",.is_optional = true},	// access to UID/GID of extrausers (if available)
			{},
		};
		char rootfs_dir[PATH_MAX] = { 0 };
		sc_must_snprintf(rootfs_dir, sizeof rootfs_dir,
				 "%s/%s/current/", SNAP_MOUNT_DIR,
				 base_snap_name);
		if (access(rootfs_dir, F_OK) != 0) {
			if (sc_streq(base_snap_name, "core")) {
				// As a special fallback, allow the
				// base snap to degrade from "core" to
				// "ubuntu-core". This is needed for
				// the migration tests.
				base_snap_name = "ubuntu-core";
				sc_must_snprintf(rootfs_dir, sizeof rootfs_dir,
						 "%s/%s/current/",
						 SNAP_MOUNT_DIR,
						 base_snap_name);
				if (access(rootfs_dir, F_OK) != 0) {
					die("cannot locate the core or legacy core snap (current symlink missing?)");
				}
			}
			// If after the special case handling above we are
			// still not ok, die
			if (access(rootfs_dir, F_OK) != 0)
			        die("cannot locate the base snap: %s", base_snap_name);
		}
		struct sc_mount_config normal_config = {
			.rootfs_dir = rootfs_dir,
			.mounts = mounts,
			.distro = distro,
			.normal_mode = true,
			.base_snap_name = base_snap_name,
		};
		sc_bootstrap_mount_namespace(&normal_config);
	} else {
		// In legacy mode we don't pivot and instead just arrange bi-
		// directional mount propagation for two directories.
		const struct sc_mount mounts[] = {
			{"/media", true},
			{"/run/netns", true},
			{},
		};
		struct sc_mount_config legacy_config = {
			.rootfs_dir = "/",
			.mounts = mounts,
			.distro = distro,
			.normal_mode = false,
			.base_snap_name = base_snap_name,
		};
		sc_bootstrap_mount_namespace(&legacy_config);
	}

	// set up private mounts
	// TODO: rename this and fold it into bootstrap
	setup_private_mount(snap_name);

	// set up private /dev/pts
	// TODO: fold this into bootstrap
	setup_private_pts();

	// setup the security backend bind mounts
	sc_call_snap_update_ns(snap_update_ns_fd, snap_name, apparmor);

	// Try to re-locate back to vanilla working directory. This can fail
	// because that directory is no longer present.
	if (chdir(vanilla_cwd) != 0) {
		debug("cannot remain in %s, moving to the void directory",
		      vanilla_cwd);
		if (chdir(SC_VOID_DIR) != 0) {
			die("cannot change directory to %s", SC_VOID_DIR);
		}
		debug("successfully moved to %s", SC_VOID_DIR);
	}
}