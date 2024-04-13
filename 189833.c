static int do_proc_readlink(struct dentry *dentry, struct vfsmount *mnt,
			    char __user *buffer, int buflen)
{
	struct inode * inode;
	char *tmp = (char*)__get_free_page(GFP_TEMPORARY);
	char *path;
	int len;

	if (!tmp)
		return -ENOMEM;

	inode = dentry->d_inode;
	path = d_path(dentry, mnt, tmp, PAGE_SIZE);
	len = PTR_ERR(path);
	if (IS_ERR(path))
		goto out;
	len = tmp + PAGE_SIZE - 1 - path;

	if (len > buflen)
		len = buflen;
	if (copy_to_user(buffer, path, len))
		len = -EFAULT;
 out:
	free_page((unsigned long)tmp);
	return len;
}