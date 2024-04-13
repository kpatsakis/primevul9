xfs_sbversion_add_attr2(xfs_mount_t *mp, xfs_trans_t *tp)
{
	if ((mp->m_flags & XFS_MOUNT_ATTR2) &&
	    !(xfs_sb_version_hasattr2(&mp->m_sb))) {
		spin_lock(&mp->m_sb_lock);
		if (!xfs_sb_version_hasattr2(&mp->m_sb)) {
			xfs_sb_version_addattr2(&mp->m_sb);
			spin_unlock(&mp->m_sb_lock);
			xfs_mod_sb(tp, XFS_SB_VERSIONNUM | XFS_SB_FEATURES2);
		} else
			spin_unlock(&mp->m_sb_lock);
	}
}