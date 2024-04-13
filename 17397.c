static void follow_mount_rcu(struct nameidata *nd)
{
	while (d_mountpoint(nd->path.dentry)) {
		struct mount *mounted;
		mounted = __lookup_mnt(nd->path.mnt, nd->path.dentry, 1);
		if (!mounted)
			break;
		nd->path.mnt = &mounted->mnt;
		nd->path.dentry = mounted->mnt.mnt_root;
		nd->seq = read_seqcount_begin(&nd->path.dentry->d_seq);
	}
}