static int proc_root_link(struct inode *inode, struct dentry **dentry, struct vfsmount **mnt)
{
	struct task_struct *task = get_proc_task(inode);
	struct fs_struct *fs = NULL;
	int result = -ENOENT;

	if (task) {
		fs = get_fs_struct(task);
		put_task_struct(task);
	}
	if (fs) {
		read_lock(&fs->lock);
		*mnt = mntget(fs->rootmnt);
		*dentry = dget(fs->root);
		read_unlock(&fs->lock);
		result = 0;
		put_fs_struct(fs);
	}
	return result;
}