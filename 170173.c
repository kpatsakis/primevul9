xfs_inode_inherit_flags(
	struct xfs_inode	*ip,
	const struct xfs_inode	*pip)
{
	unsigned int		di_flags = 0;
	umode_t			mode = VFS_I(ip)->i_mode;

	if (S_ISDIR(mode)) {
		if (pip->i_d.di_flags & XFS_DIFLAG_RTINHERIT)
			di_flags |= XFS_DIFLAG_RTINHERIT;
		if (pip->i_d.di_flags & XFS_DIFLAG_EXTSZINHERIT) {
			di_flags |= XFS_DIFLAG_EXTSZINHERIT;
			ip->i_d.di_extsize = pip->i_d.di_extsize;
		}
		if (pip->i_d.di_flags & XFS_DIFLAG_PROJINHERIT)
			di_flags |= XFS_DIFLAG_PROJINHERIT;
	} else if (S_ISREG(mode)) {
		if ((pip->i_d.di_flags & XFS_DIFLAG_RTINHERIT) &&
		    xfs_sb_version_hasrealtime(&ip->i_mount->m_sb))
			di_flags |= XFS_DIFLAG_REALTIME;
		if (pip->i_d.di_flags & XFS_DIFLAG_EXTSZINHERIT) {
			di_flags |= XFS_DIFLAG_EXTSIZE;
			ip->i_d.di_extsize = pip->i_d.di_extsize;
		}
	}
	if ((pip->i_d.di_flags & XFS_DIFLAG_NOATIME) &&
	    xfs_inherit_noatime)
		di_flags |= XFS_DIFLAG_NOATIME;
	if ((pip->i_d.di_flags & XFS_DIFLAG_NODUMP) &&
	    xfs_inherit_nodump)
		di_flags |= XFS_DIFLAG_NODUMP;
	if ((pip->i_d.di_flags & XFS_DIFLAG_SYNC) &&
	    xfs_inherit_sync)
		di_flags |= XFS_DIFLAG_SYNC;
	if ((pip->i_d.di_flags & XFS_DIFLAG_NOSYMLINKS) &&
	    xfs_inherit_nosymlinks)
		di_flags |= XFS_DIFLAG_NOSYMLINKS;
	if ((pip->i_d.di_flags & XFS_DIFLAG_NODEFRAG) &&
	    xfs_inherit_nodefrag)
		di_flags |= XFS_DIFLAG_NODEFRAG;
	if (pip->i_d.di_flags & XFS_DIFLAG_FILESTREAM)
		di_flags |= XFS_DIFLAG_FILESTREAM;

	ip->i_d.di_flags |= di_flags;
}