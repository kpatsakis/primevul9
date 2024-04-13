static int compat_filldir64(void * __buf, const char * name, int namlen, loff_t offset,
		     u64 ino, unsigned int d_type)
{
	struct linux_dirent64 __user *dirent;
	struct compat_getdents_callback64 *buf = __buf;
	int jj = NAME_OFFSET(dirent);
	int reclen = COMPAT_ROUND_UP64(jj + namlen + 1);
	u64 off;

	buf->error = -EINVAL;	/* only used if we fail.. */
	if (reclen > buf->count)
		return -EINVAL;
	dirent = buf->previous;

	if (dirent) {
		if (__put_user_unaligned(offset, &dirent->d_off))
			goto efault;
	}
	dirent = buf->current_dir;
	if (__put_user_unaligned(ino, &dirent->d_ino))
		goto efault;
	off = 0;
	if (__put_user_unaligned(off, &dirent->d_off))
		goto efault;
	if (__put_user(reclen, &dirent->d_reclen))
		goto efault;
	if (__put_user(d_type, &dirent->d_type))
		goto efault;
	if (copy_to_user(dirent->d_name, name, namlen))
		goto efault;
	if (__put_user(0, dirent->d_name + namlen))
		goto efault;
	buf->previous = dirent;
	dirent = (void __user *)dirent + reclen;
	buf->current_dir = dirent;
	buf->count -= reclen;
	return 0;
efault:
	buf->error = -EFAULT;
	return -EFAULT;
}