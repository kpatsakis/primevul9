static int bad_inode_getattr(struct vfsmount *mnt, struct dentry *dentry,
			struct kstat *stat)
{
	return -EIO;
}