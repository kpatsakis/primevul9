xfs_bmapi_reserve_delalloc(
	struct xfs_inode	*ip,
	int			whichfork,
	xfs_fileoff_t		off,
	xfs_filblks_t		len,
	xfs_filblks_t		prealloc,
	struct xfs_bmbt_irec	*got,
	struct xfs_iext_cursor	*icur,
	int			eof)
{
	struct xfs_mount	*mp = ip->i_mount;
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(ip, whichfork);
	xfs_extlen_t		alen;
	xfs_extlen_t		indlen;
	int			error;
	xfs_fileoff_t		aoff = off;

	/*
	 * Cap the alloc length. Keep track of prealloc so we know whether to
	 * tag the inode before we return.
	 */
	alen = XFS_FILBLKS_MIN(len + prealloc, MAXEXTLEN);
	if (!eof)
		alen = XFS_FILBLKS_MIN(alen, got->br_startoff - aoff);
	if (prealloc && alen >= len)
		prealloc = alen - len;

	/* Figure out the extent size, adjust alen */
	if (whichfork == XFS_COW_FORK) {
		struct xfs_bmbt_irec	prev;
		xfs_extlen_t		extsz = xfs_get_cowextsz_hint(ip);

		if (!xfs_iext_peek_prev_extent(ifp, icur, &prev))
			prev.br_startoff = NULLFILEOFF;

		error = xfs_bmap_extsize_align(mp, got, &prev, extsz, 0, eof,
					       1, 0, &aoff, &alen);
		ASSERT(!error);
	}

	/*
	 * Make a transaction-less quota reservation for delayed allocation
	 * blocks.  This number gets adjusted later.  We return if we haven't
	 * allocated blocks already inside this loop.
	 */
	error = xfs_trans_reserve_quota_nblks(NULL, ip, (long)alen, 0,
						XFS_QMOPT_RES_REGBLKS);
	if (error)
		return error;

	/*
	 * Split changing sb for alen and indlen since they could be coming
	 * from different places.
	 */
	indlen = (xfs_extlen_t)xfs_bmap_worst_indlen(ip, alen);
	ASSERT(indlen > 0);

	error = xfs_mod_fdblocks(mp, -((int64_t)alen), false);
	if (error)
		goto out_unreserve_quota;

	error = xfs_mod_fdblocks(mp, -((int64_t)indlen), false);
	if (error)
		goto out_unreserve_blocks;


	ip->i_delayed_blks += alen;

	got->br_startoff = aoff;
	got->br_startblock = nullstartblock(indlen);
	got->br_blockcount = alen;
	got->br_state = XFS_EXT_NORM;

	xfs_bmap_add_extent_hole_delay(ip, whichfork, icur, got);

	/*
	 * Tag the inode if blocks were preallocated. Note that COW fork
	 * preallocation can occur at the start or end of the extent, even when
	 * prealloc == 0, so we must also check the aligned offset and length.
	 */
	if (whichfork == XFS_DATA_FORK && prealloc)
		xfs_inode_set_eofblocks_tag(ip);
	if (whichfork == XFS_COW_FORK && (prealloc || aoff < off || alen > len))
		xfs_inode_set_cowblocks_tag(ip);

	return 0;

out_unreserve_blocks:
	xfs_mod_fdblocks(mp, alen, false);
out_unreserve_quota:
	if (XFS_IS_QUOTA_ON(mp))
		xfs_trans_unreserve_quota_nblks(NULL, ip, (long)alen, 0,
						XFS_QMOPT_RES_REGBLKS);
	return error;
}