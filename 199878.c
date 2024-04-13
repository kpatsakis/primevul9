asmlinkage long compat_sys_ioctl(unsigned int fd, unsigned int cmd,
				unsigned long arg)
{
	struct file *filp;
	int error = -EBADF;
	struct ioctl_trans *t;
	int fput_needed;

	filp = fget_light(fd, &fput_needed);
	if (!filp)
		goto out;

	/* RED-PEN how should LSM module know it's handling 32bit? */
	error = security_file_ioctl(filp, cmd, arg);
	if (error)
		goto out_fput;

	/*
	 * To allow the compat_ioctl handlers to be self contained
	 * we need to check the common ioctls here first.
	 * Just handle them with the standard handlers below.
	 */
	switch (cmd) {
	case FIOCLEX:
	case FIONCLEX:
	case FIONBIO:
	case FIOASYNC:
	case FIOQSIZE:
		break;

	case FIBMAP:
	case FIGETBSZ:
	case FIONREAD:
		if (S_ISREG(filp->f_dentry->d_inode->i_mode))
			break;
		/*FALL THROUGH*/

	default:
		if (filp->f_op && filp->f_op->compat_ioctl) {
			error = filp->f_op->compat_ioctl(filp, cmd, arg);
			if (error != -ENOIOCTLCMD)
				goto out_fput;
		}

		if (!filp->f_op ||
		    (!filp->f_op->ioctl && !filp->f_op->unlocked_ioctl))
			goto do_ioctl;
		break;
	}

	for (t = ioctl32_hash_table[ioctl32_hash(cmd)]; t; t = t->next) {
		if (t->cmd == cmd)
			goto found_handler;
	}

	if (S_ISSOCK(filp->f_dentry->d_inode->i_mode) &&
	    cmd >= SIOCDEVPRIVATE && cmd <= (SIOCDEVPRIVATE + 15)) {
		error = siocdevprivate_ioctl(fd, cmd, arg);
	} else {
		static int count;

		if (++count <= 50)
			compat_ioctl_error(filp, fd, cmd, arg);
		error = -EINVAL;
	}

	goto out_fput;

 found_handler:
	if (t->handler) {
		lock_kernel();
		error = t->handler(fd, cmd, arg, filp);
		unlock_kernel();
		goto out_fput;
	}

 do_ioctl:
	error = vfs_ioctl(filp, fd, cmd, arg);
 out_fput:
	fput_light(filp, fput_needed);
 out:
	return error;
}