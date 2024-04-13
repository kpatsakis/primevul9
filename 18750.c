static int mk_exit_code(struct libmnt_context *cxt, int rc)
{
	int syserr;
	struct stat st;
	unsigned long uflags = 0, mflags = 0;

	int restricted = mnt_context_is_restricted(cxt);
	const char *tgt = mnt_context_get_target(cxt);
	const char *src = mnt_context_get_source(cxt);

try_readonly:
	if (mnt_context_helper_executed(cxt))
		/*
		 * /sbin/mount.<type> called, return status
		 */
		return mnt_context_get_helper_status(cxt);

	if (rc == 0 && mnt_context_get_status(cxt) == 1) {
		/*
		 * Libmount success && syscall success.
		 */
		selinux_warning(cxt, tgt);

		return MOUNT_EX_SUCCESS;	/* mount(2) success */
	}

	mnt_context_get_mflags(cxt, &mflags);		/* mount(2) flags */
	mnt_context_get_user_mflags(cxt, &uflags);	/* userspace flags */

	if (!mnt_context_syscall_called(cxt)) {
		/*
		 * libmount errors (extra library checks)
		 */
		switch (rc) {
		case -EPERM:
			warnx(_("only root can mount %s on %s"), src, tgt);
			return MOUNT_EX_USAGE;
		case -EBUSY:
			warnx(_("%s is already mounted"), src);
			return MOUNT_EX_USAGE;
		case -MNT_ERR_NOFSTAB:
			if (mnt_context_is_swapmatch(cxt)) {
				warnx(_("can't find %s in %s"),
						src ? src : tgt,
						mnt_get_fstab_path());
				return MOUNT_EX_USAGE;
			}
			/* source/target explicitly defined */
			if (tgt)
				warnx(_("can't find mountpoint %s in %s"),
						tgt, mnt_get_fstab_path());
			else
				warnx(_("can't find mount source %s in %s"),
						src, mnt_get_fstab_path());
			return MOUNT_EX_USAGE;
		case -MNT_ERR_NOFSTYPE:
			if (restricted)
				warnx(_("I could not determine the filesystem type, "
					"and none was specified"));
			else
				warnx(_("you must specify the filesystem type"));
			return MOUNT_EX_USAGE;
		case -MNT_ERR_NOSOURCE:
			if (src)
				warnx(_("can't find %s"), src);
			else
				warnx(_("mount source not defined"));
			return MOUNT_EX_USAGE;
		case -MNT_ERR_MOUNTOPT:
			if (errno)
				warn(_("failed to parse mount options"));
			else
				warnx(_("failed to parse mount options"));
			return MOUNT_EX_USAGE;
		case -MNT_ERR_LOOPDEV:
			warn(_("%s: failed to setup loop device"), src);
			return MOUNT_EX_FAIL;
		default:
			return handle_generic_errors(rc, _("%s: mount failed"),
					     tgt ? tgt : src);
		}
	} else if (mnt_context_get_syscall_errno(cxt) == 0) {
		/*
		 * mount(2) syscall success, but something else failed
		 * (probably error in mtab processing).
		 */
		if (rc < 0)
			return handle_generic_errors(rc,
				_("%s: filesystem mounted, but mount(8) failed"),
				tgt ? tgt : src);

		return MOUNT_EX_SOFTWARE;	/* internal error */

	}

	/*
	 * mount(2) errors
	 */
	syserr = mnt_context_get_syscall_errno(cxt);


	switch(syserr) {
	case EPERM:
		if (geteuid() == 0) {
			if (stat(tgt, &st) || !S_ISDIR(st.st_mode))
				warnx(_("mount point %s is not a directory"), tgt);
			else
				warnx(_("permission denied"));
		} else
			warnx(_("must be superuser to use mount"));
	      break;

	case EBUSY:
	{
		struct libmnt_table *tb;

		if (mflags & MS_REMOUNT) {
			warnx(_("%s is busy"), tgt);
			break;
		}

		warnx(_("%s is already mounted or %s busy"), src, tgt);

		if (src && mnt_context_get_mtab(cxt, &tb) == 0) {
			struct libmnt_iter *itr = mnt_new_iter(MNT_ITER_FORWARD);
			struct libmnt_fs *fs;

			while(mnt_table_next_fs(tb, itr, &fs) == 0) {
				const char *s = mnt_fs_get_srcpath(fs),
					   *t = mnt_fs_get_target(fs);

				if (t && s && mnt_fs_streq_srcpath(fs, src))
					fprintf(stderr, _(
						"       %s is already mounted on %s\n"), s, t);
			}
			mnt_free_iter(itr);
		}
		break;
	}
	case ENOENT:
		if (lstat(tgt, &st))
			warnx(_("mount point %s does not exist"), tgt);
		else if (stat(tgt, &st))
			warnx(_("mount point %s is a symbolic link to nowhere"), tgt);
		else if (stat(src, &st)) {
			if (uflags & MNT_MS_NOFAIL)
				return MOUNT_EX_SUCCESS;

			warnx(_("special device %s does not exist"), src);
		} else {
			errno = syserr;
			warn(_("mount(2) failed"));	/* print errno */
		}
		break;

	case ENOTDIR:
		if (stat(tgt, &st) || ! S_ISDIR(st.st_mode))
			warnx(_("mount point %s is not a directory"), tgt);
		else if (stat(src, &st) && errno == ENOTDIR) {
			if (uflags & MNT_MS_NOFAIL)
				return MOUNT_EX_SUCCESS;

			warnx(_("special device %s does not exist "
				 "(a path prefix is not a directory)"), src);
		} else {
			errno = syserr;
			warn(_("mount(2) failed"));     /* print errno */
		}
		break;

	case EINVAL:
		if (mflags & MS_REMOUNT)
			warnx(_("%s not mounted or bad option"), tgt);
		else if (mflags & MS_PROPAGATION)
			warnx(_("%s is not mountpoint or bad option"), tgt);
		else
			warnx(_("wrong fs type, bad option, bad superblock on %s,\n"
				"       missing codepage or helper program, or other error"),
				src);

		if (mnt_fs_is_netfs(mnt_context_get_fs(cxt)))
			fprintf(stderr, _(
				"       (for several filesystems (e.g. nfs, cifs) you might\n"
				"       need a /sbin/mount.<type> helper program)\n"));

		fprintf(stderr, _(
				"       In some cases useful info is found in syslog - try\n"
				"       dmesg | tail or so\n"));
		break;

	case EMFILE:
		warnx(_("mount table full"));
		break;

	case EIO:
		warnx(_("%s: can't read superblock"), src);
		break;

	case ENODEV:
		warnx(_("unknown filesystem type '%s'"), mnt_context_get_fstype(cxt));
		break;

	case ENOTBLK:
		if (uflags & MNT_MS_NOFAIL)
			return MOUNT_EX_SUCCESS;

		if (stat(src, &st))
			warnx(_("%s is not a block device, and stat(2) fails?"), src);
		else if (S_ISBLK(st.st_mode))
			warnx(_("the kernel does not recognize %s as a block device\n"
				"       (maybe `modprobe driver'?)"), src);
		else if (S_ISREG(st.st_mode))
			warnx(_("%s is not a block device (maybe try `-o loop'?)"), src);
		else
			warnx(_(" %s is not a block device"), src);
		break;

	case ENXIO:
		if (uflags & MNT_MS_NOFAIL)
			return MOUNT_EX_SUCCESS;

		warnx(_("%s is not a valid block device"), src);
		break;

	case EACCES:
	case EROFS:
		if (mflags & MS_RDONLY)
			warnx(_("cannot mount %s read-only"), src);

		else if (readwrite)
			warnx(_("%s is write-protected but explicit `-w' flag given"), src);

		else if (mflags & MS_REMOUNT)
			warnx(_("cannot remount %s read-write, is write-protected"), src);

		else {
			warnx(_("%s is write-protected, mounting read-only"), src);

			mnt_context_reset_status(cxt);
			mnt_context_set_mflags(cxt, mflags | MS_RDONLY);
			rc = mnt_context_do_mount(cxt);
			if (!rc)
				rc = mnt_context_finalize_mount(cxt);

			goto try_readonly;
		}
		break;

	case ENOMEDIUM:
		warnx(_("no medium found on %s"), src);
		break;

	default:
		warn(_("mount %s on %s failed"), src, tgt);
		break;
	}

	return MOUNT_EX_FAIL;
}