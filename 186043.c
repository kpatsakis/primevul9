static long ioctl_file_clone(struct file *dst_file, unsigned long srcfd,
			     u64 off, u64 olen, u64 destoff)
{
	struct fd src_file = fdget(srcfd);
	int ret;

	if (!src_file.file)
		return -EBADF;
	ret = vfs_clone_file_range(src_file.file, off, dst_file, destoff, olen);
	fdput(src_file);
	return ret;
}