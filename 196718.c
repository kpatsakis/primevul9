xfs_fs_show_options(
	struct seq_file		*m,
	struct dentry		*root)
{
	return xfs_showargs(XFS_M(root->d_sb), m);
}