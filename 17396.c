int follow_up(struct path *path)
{
	struct mount *mnt = real_mount(path->mnt);
	struct mount *parent;
	struct dentry *mountpoint;

	br_read_lock(&vfsmount_lock);
	parent = mnt->mnt_parent;
	if (parent == mnt) {
		br_read_unlock(&vfsmount_lock);
		return 0;
	}
	mntget(&parent->mnt);
	mountpoint = dget(mnt->mnt_mountpoint);
	br_read_unlock(&vfsmount_lock);
	dput(path->dentry);
	path->dentry = mountpoint;
	mntput(path->mnt);
	path->mnt = &parent->mnt;
	return 1;
}