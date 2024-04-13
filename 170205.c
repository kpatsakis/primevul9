xfs_rename_alloc_whiteout(
	struct xfs_inode	*dp,
	struct xfs_inode	**wip)
{
	struct xfs_inode	*tmpfile;
	int			error;

	error = xfs_create_tmpfile(dp, S_IFCHR | WHITEOUT_MODE, &tmpfile);
	if (error)
		return error;

	/*
	 * Prepare the tmpfile inode as if it were created through the VFS.
	 * Complete the inode setup and flag it as linkable.  nlink is already
	 * zero, so we can skip the drop_nlink.
	 */
	xfs_setup_iops(tmpfile);
	xfs_finish_inode_setup(tmpfile);
	VFS_I(tmpfile)->i_state |= I_LINKABLE;

	*wip = tmpfile;
	return 0;
}