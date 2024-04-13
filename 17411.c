SYSCALL_DEFINE3(fchown, unsigned int, fd, uid_t, user, gid_t, group)
{
	struct fd f = fdget(fd);
	int error = -EBADF;

	if (!f.file)
		goto out;

	error = mnt_want_write_file(f.file);
	if (error)
		goto out_fput;
	audit_inode(NULL, f.file->f_path.dentry, 0);
	error = chown_common(&f.file->f_path, user, group);
	mnt_drop_write_file(f.file);
out_fput:
	fdput(f);
out:
	return error;
}