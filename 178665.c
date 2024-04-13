static int mk_exit_code(struct libmnt_context *cxt, int rc)
{
	int syserr;
	const char *tgt = mnt_context_get_target(cxt);

	if (mnt_context_helper_executed(cxt))
		/*
		 * /sbin/umount.<type> called, return status
		 */
		return mnt_context_get_helper_status(cxt);

	if (rc == 0 && mnt_context_get_status(cxt) == 1)
		/*
		 * Libmount success && syscall success.
		 */
		return MOUNT_EX_SUCCESS;


	if (!mnt_context_syscall_called(cxt)) {
		/*
		 * libmount errors (extra library checks)
		 */
		return handle_generic_errors(rc, _("%s: umount failed"), tgt);

	} else if (mnt_context_get_syscall_errno(cxt) == 0) {
		/*
		 * umount(2) syscall success, but something else failed
		 * (probably error in mtab processing).
		 */
		if (rc < 0)
			return handle_generic_errors(rc,
				_("%s: filesystem umounted, but mount(8) failed"),
				tgt);

		return MOUNT_EX_SOFTWARE;	/* internal error */

	}

	/*
	 * umount(2) errors
	 */
	syserr = mnt_context_get_syscall_errno(cxt);

	switch(syserr) {
	case ENXIO:
		warnx(_("%s: invalid block device"), tgt);	/* ??? */
		break;
	case EINVAL:
		warnx(_("%s: not mounted"), tgt);
		break;
	case EIO:
		warnx(_("%s: can't write superblock"), tgt);
		break;
	case EBUSY:
		warnx(_("%s: target is busy.\n"
		       "        (In some cases useful info about processes that use\n"
		       "         the device is found by lsof(8) or fuser(1))"),
			tgt);
		break;
	case ENOENT:
		warnx(_("%s: not found"), tgt);
		break;
	case EPERM:
		warnx(_("%s: must be superuser to umount"), tgt);
		break;
	case EACCES:
		warnx(_("%s: block devices not permitted on fs"), tgt);
		break;
	default:
		errno = syserr;
		warn(_("%s"), tgt);
		break;
	}
	return MOUNT_EX_FAIL;
}