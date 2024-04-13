static void sc_bootstrap_mount_namespace(const struct sc_mount_config *config)
{
	char scratch_dir[] = "/tmp/snap.rootfs_XXXXXX";
	char src[PATH_MAX] = { 0 };
	char dst[PATH_MAX] = { 0 };
	if (mkdtemp(scratch_dir) == NULL) {
		die("cannot create temporary directory for the root file system");
	}
	// NOTE: at this stage we just called unshare(CLONE_NEWNS). We are in a new
	// mount namespace and have a private list of mounts.
	debug("scratch directory for constructing namespace: %s", scratch_dir);
	// Make the root filesystem recursively shared. This way propagation events
	// will be shared with main mount namespace.
	sc_do_mount("none", "/", NULL, MS_REC | MS_SHARED, NULL);
	// Bind mount the temporary scratch directory for root filesystem over
	// itself so that it is a mount point. This is done so that it can become
	// unbindable as explained below.
	sc_do_mount(scratch_dir, scratch_dir, NULL, MS_BIND, NULL);
	// Make the scratch directory unbindable.
	//
	// This is necessary as otherwise a mount loop can occur and the kernel
	// would crash. The term unbindable simply states that it cannot be bind
	// mounted anywhere. When we construct recursive bind mounts below this
	// guarantees that this directory will not be replicated anywhere.
	sc_do_mount("none", scratch_dir, NULL, MS_UNBINDABLE, NULL);
	// Recursively bind mount desired root filesystem directory over the
	// scratch directory. This puts the initial content into the scratch space
	// and serves as a foundation for all subsequent operations below.
	//
	// The mount is recursive because it can either be applied to the root
	// filesystem of a core system (aka all-snap) or the core snap on a classic
	// system. In the former case we need recursive bind mounts to accurately
	// replicate the state of the root filesystem into the scratch directory.
	sc_do_mount(config->rootfs_dir, scratch_dir, NULL, MS_REC | MS_BIND,
		    NULL);
	// Make the scratch directory recursively private. Nothing done there will
	// be shared with any peer group, This effectively detaches us from the
	// original namespace and coupled with pivot_root below serves as the
	// foundation of the mount sandbox.
	sc_do_mount("none", scratch_dir, NULL, MS_REC | MS_SLAVE, NULL);
	// Bind mount certain directories from the host filesystem to the scratch
	// directory. By default mount events will propagate in both into and out
	// of the peer group. This way the running application can alter any global
	// state visible on the host and in other snaps. This can be restricted by
	// disabling the "is_bidirectional" flag as can be seen below.
	for (const struct sc_mount * mnt = config->mounts; mnt->path != NULL;
	     mnt++) {
		if (mnt->is_bidirectional && mkdir(mnt->path, 0755) < 0 &&
		    errno != EEXIST) {
			die("cannot create %s", mnt->path);
		}
		sc_must_snprintf(dst, sizeof dst, "%s/%s", scratch_dir,
				 mnt->path);
		if (mnt->is_optional) {
			bool ok = sc_do_optional_mount(mnt->path, dst, NULL,
						       MS_REC | MS_BIND, NULL);
			if (!ok) {
				// If we cannot mount it, just continue.
				continue;
			}
		} else {
			sc_do_mount(mnt->path, dst, NULL, MS_REC | MS_BIND,
				    NULL);
		}
		if (!mnt->is_bidirectional) {
			// Mount events will only propagate inwards to the namespace. This
			// way the running application cannot alter any global state apart
			// from that of its own snap.
			sc_do_mount("none", dst, NULL, MS_REC | MS_SLAVE, NULL);
		}
		if (mnt->altpath == NULL) {
			continue;
		}
		// An alternate path of mnt->path is provided at another location.
		// It should behave exactly the same as the original.
		sc_must_snprintf(dst, sizeof dst, "%s/%s", scratch_dir,
				 mnt->altpath);
		struct stat stat_buf;
		if (lstat(dst, &stat_buf) < 0) {
			die("cannot lstat %s", dst);
		}
		if ((stat_buf.st_mode & S_IFMT) == S_IFLNK) {
			die("cannot bind mount alternate path over a symlink: %s", dst);
		}
		sc_do_mount(mnt->path, dst, NULL, MS_REC | MS_BIND, NULL);
		if (!mnt->is_bidirectional) {
			sc_do_mount("none", dst, NULL, MS_REC | MS_SLAVE, NULL);
		}
	}
	if (config->normal_mode) {
		// Since we mounted /etc from the host filesystem to the scratch directory,
		// we may need to put certain directories from the desired root filesystem
		// (e.g. the core snap) back. This way the behavior of running snaps is not
		// affected by the alternatives directory from the host, if one exists.
		//
		// Fixes the following bugs:
		//  - https://bugs.launchpad.net/snap-confine/+bug/1580018
		//  - https://bugzilla.opensuse.org/show_bug.cgi?id=1028568
		const char *dirs_from_core[] =
		    { "/etc/alternatives", "/etc/ssl", "/etc/nsswitch.conf",
			NULL
		};
		for (const char **dirs = dirs_from_core; *dirs != NULL; dirs++) {
			const char *dir = *dirs;
			struct stat buf;
			if (access(dir, F_OK) == 0) {
				sc_must_snprintf(src, sizeof src, "%s%s",
						 config->rootfs_dir, dir);
				sc_must_snprintf(dst, sizeof dst, "%s%s",
						 scratch_dir, dir);
				if (lstat(src, &buf) == 0
				    && lstat(dst, &buf) == 0) {
					sc_do_mount(src, dst, NULL, MS_BIND,
						    NULL);
					sc_do_mount("none", dst, NULL, MS_SLAVE,
						    NULL);
				}
			}
		}
	}
	// The "core" base snap is special as it contains snapd and friends.
	// Other base snaps do not, so whenever a base snap other than core is
	// in use we need extra provisions for setting up internal tooling to
	// be available.
	//
	// However on a core18 (and similar) system the core snap is not
	// a special base anymore and we should map our own tooling in.
	if (config->distro == SC_DISTRO_CORE_OTHER
	    || !sc_streq(config->base_snap_name, "core")) {
		// when bases are used we need to bind-mount the libexecdir
		// (that contains snap-exec) into /usr/lib/snapd of the
		// base snap so that snap-exec is available for the snaps
		// (base snaps do not ship snapd)

		// dst is always /usr/lib/snapd as this is where snapd
		// assumes to find snap-exec
		sc_must_snprintf(dst, sizeof dst, "%s/usr/lib/snapd",
				 scratch_dir);

		// bind mount the current $ROOT/usr/lib/snapd path,
		// where $ROOT is either "/" or the "/snap/{core,snapd}/current"
		// that we are re-execing from
		char *src = NULL;
		char self[PATH_MAX + 1] = { 0 };
		if (readlink("/proc/self/exe", self, sizeof(self) - 1) < 0) {
			die("cannot read /proc/self/exe");
		}
		// this cannot happen except when the kernel is buggy
		if (strstr(self, "/snap-confine") == NULL) {
			die("cannot use result from readlink: %s", self);
		}
		src = dirname(self);
		// dirname(path) might return '.' depending on path.
		// /proc/self/exe should always point
		// to an absolute path, but let's guarantee that.
		if (src[0] != '/') {
			die("cannot use the result of dirname(): %s", src);
		}

		sc_do_mount(src, dst, NULL, MS_BIND | MS_RDONLY, NULL);
		sc_do_mount("none", dst, NULL, MS_SLAVE, NULL);
	}
	// Bind mount the directory where all snaps are mounted. The location of
	// the this directory on the host filesystem may not match the location in
	// the desired root filesystem. In the "core" and "ubuntu-core" snaps the
	// directory is always /snap. On the host it is a build-time configuration
	// option stored in SNAP_MOUNT_DIR.
	sc_must_snprintf(dst, sizeof dst, "%s/snap", scratch_dir);
	sc_do_mount(SNAP_MOUNT_DIR, dst, NULL, MS_BIND | MS_REC | MS_SLAVE,
		    NULL);
	sc_do_mount("none", dst, NULL, MS_REC | MS_SLAVE, NULL);
	// Create the hostfs directory if one is missing. This directory is a part
	// of packaging now so perhaps this code can be removed later.
	if (access(SC_HOSTFS_DIR, F_OK) != 0) {
		debug("creating missing hostfs directory");
		if (mkdir(SC_HOSTFS_DIR, 0755) != 0) {
			die("cannot perform operation: mkdir %s",
			    SC_HOSTFS_DIR);
		}
	}
	// Ensure that hostfs isgroup owned by root. We may have (now or earlier)
	// created the directory as the user who first ran a snap on a given
	// system and the group identity of that user is visilbe on disk.
	// This was LP:#1665004
	struct stat sb;
	if (stat(SC_HOSTFS_DIR, &sb) < 0) {
		die("cannot stat %s", SC_HOSTFS_DIR);
	}
	if (sb.st_uid != 0 || sb.st_gid != 0) {
		if (chown(SC_HOSTFS_DIR, 0, 0) < 0) {
			die("cannot change user/group owner of %s to root",
			    SC_HOSTFS_DIR);
		}
	}
	// Make the upcoming "put_old" directory for pivot_root private so that
	// mount events don't propagate to any peer group. In practice pivot root
	// has a number of undocumented requirements and one of them is that the
	// "put_old" directory (the second argument) cannot be shared in any way.
	sc_must_snprintf(dst, sizeof dst, "%s/%s", scratch_dir, SC_HOSTFS_DIR);
	sc_do_mount(dst, dst, NULL, MS_BIND, NULL);
	sc_do_mount("none", dst, NULL, MS_PRIVATE, NULL);
	// On classic mount the nvidia driver. Ideally this would be done in an
	// uniform way after pivot_root but this is good enough and requires less
	// code changes the nvidia code assumes it has access to the existing
	// pre-pivot filesystem.
	if (config->distro == SC_DISTRO_CLASSIC) {
		sc_mount_nvidia_driver(scratch_dir);
	}
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	//                    pivot_root
	// XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
	// Use pivot_root to "chroot" into the scratch directory.
	//
	// Q: Why are we using something as esoteric as pivot_root(2)?
	// A: Because this makes apparmor handling easy. Using a normal chroot
	// makes all apparmor rules conditional.  We are either running on an
	// all-snap system where this would-be chroot didn't happen and all the
	// rules see / as the root file system _OR_ we are running on top of a
	// classic distribution and this chroot has now moved all paths to
	// /tmp/snap.rootfs_*.
	//
	// Because we are using unshare(2) with CLONE_NEWNS we can essentially use
	// pivot_root just like chroot but this makes apparmor unaware of the old
	// root so everything works okay.
	//
	// HINT: If you are debugging this and are trying to see why pivot_root
	// happens to return EINVAL with any changes you may be making, please
	// consider applying
	// misc/0001-Add-printk-based-debugging-to-pivot_root.patch to your tree
	// kernel.
	debug("performing operation: pivot_root %s %s", scratch_dir, dst);
	if (syscall(SYS_pivot_root, scratch_dir, dst) < 0) {
		die("cannot perform operation: pivot_root %s %s", scratch_dir,
		    dst);
	}
	// Unmount the self-bind mount over the scratch directory created earlier
	// in the original root filesystem (which is now mounted on SC_HOSTFS_DIR).
	// This way we can remove the temporary directory we created and "clean up"
	// after ourselves nicely.
	sc_must_snprintf(dst, sizeof dst, "%s/%s", SC_HOSTFS_DIR, scratch_dir);
	sc_do_umount(dst, 0);
	// Remove the scratch directory. Note that we are using the path that is
	// based on the old root filesystem as after pivot_root we cannot guarantee
	// what is present at the same location normally. (It is probably an empty
	// /tmp directory that is populated in another place).
	debug("performing operation: rmdir %s", dst);
	if (rmdir(scratch_dir) < 0) {
		die("cannot perform operation: rmdir %s", dst);
	};
	// Make the old root filesystem recursively slave. This way operations
	// performed in this mount namespace will not propagate to the peer group.
	// This is another essential part of the confinement system.
	sc_do_mount("none", SC_HOSTFS_DIR, NULL, MS_REC | MS_SLAVE, NULL);
	// Detach the redundant hostfs version of sysfs since it shows up in the
	// mount table and software inspecting the mount table may become confused
	// (eg, docker and LP:# 162601).
	sc_must_snprintf(src, sizeof src, "%s/sys", SC_HOSTFS_DIR);
	sc_do_umount(src, UMOUNT_NOFOLLOW | MNT_DETACH);
	// Detach the redundant hostfs version of /dev since it shows up in the
	// mount table and software inspecting the mount table may become confused.
	sc_must_snprintf(src, sizeof src, "%s/dev", SC_HOSTFS_DIR);
	sc_do_umount(src, UMOUNT_NOFOLLOW | MNT_DETACH);
	// Detach the redundant hostfs version of /proc since it shows up in the
	// mount table and software inspecting the mount table may become confused.
	sc_must_snprintf(src, sizeof src, "%s/proc", SC_HOSTFS_DIR);
	sc_do_umount(src, UMOUNT_NOFOLLOW | MNT_DETACH);
}