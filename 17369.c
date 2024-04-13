static int follow_up_rcu(struct path *path)
{
	struct mount *mnt = real_mount(path->mnt);
	struct mount *parent;
	struct dentry *mountpoint;

	parent = mnt->mnt_parent;
	if (&parent->mnt == path->mnt)
		return 0;
	mountpoint = mnt->mnt_mountpoint;
	path->dentry = mountpoint;
	path->mnt = &parent->mnt;
	return 1;
}