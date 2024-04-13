xfs_bmap_alloc(
	struct xfs_bmalloca	*ap)	/* bmap alloc argument struct */
{
	if (XFS_IS_REALTIME_INODE(ap->ip) &&
	    xfs_alloc_is_userdata(ap->datatype))
		return xfs_bmap_rtalloc(ap);
	return xfs_bmap_btalloc(ap);
}