xfs_bmapi_allocate(
	struct xfs_bmalloca	*bma)
{
	struct xfs_mount	*mp = bma->ip->i_mount;
	int			whichfork = xfs_bmapi_whichfork(bma->flags);
	struct xfs_ifork	*ifp = XFS_IFORK_PTR(bma->ip, whichfork);
	int			tmp_logflags = 0;
	int			error;

	ASSERT(bma->length > 0);

	/*
	 * For the wasdelay case, we could also just allocate the stuff asked
	 * for in this bmap call but that wouldn't be as good.
	 */
	if (bma->wasdel) {
		bma->length = (xfs_extlen_t)bma->got.br_blockcount;
		bma->offset = bma->got.br_startoff;
		xfs_iext_peek_prev_extent(ifp, &bma->icur, &bma->prev);
	} else {
		bma->length = XFS_FILBLKS_MIN(bma->length, MAXEXTLEN);
		if (!bma->eof)
			bma->length = XFS_FILBLKS_MIN(bma->length,
					bma->got.br_startoff - bma->offset);
	}

	/*
	 * Set the data type being allocated. For the data fork, the first data
	 * in the file is treated differently to all other allocations. For the
	 * attribute fork, we only need to ensure the allocated range is not on
	 * the busy list.
	 */
	if (!(bma->flags & XFS_BMAPI_METADATA)) {
		bma->datatype = XFS_ALLOC_NOBUSY;
		if (whichfork == XFS_DATA_FORK) {
			if (bma->offset == 0)
				bma->datatype |= XFS_ALLOC_INITIAL_USER_DATA;
			else
				bma->datatype |= XFS_ALLOC_USERDATA;
		}
		if (bma->flags & XFS_BMAPI_ZERO)
			bma->datatype |= XFS_ALLOC_USERDATA_ZERO;
	}

	bma->minlen = (bma->flags & XFS_BMAPI_CONTIG) ? bma->length : 1;

	/*
	 * Only want to do the alignment at the eof if it is userdata and
	 * allocation length is larger than a stripe unit.
	 */
	if (mp->m_dalign && bma->length >= mp->m_dalign &&
	    !(bma->flags & XFS_BMAPI_METADATA) && whichfork == XFS_DATA_FORK) {
		error = xfs_bmap_isaeof(bma, whichfork);
		if (error)
			return error;
	}

	error = xfs_bmap_alloc(bma);
	if (error)
		return error;

	if (bma->cur)
		bma->cur->bc_private.b.firstblock = *bma->firstblock;
	if (bma->blkno == NULLFSBLOCK)
		return 0;
	if ((ifp->if_flags & XFS_IFBROOT) && !bma->cur) {
		bma->cur = xfs_bmbt_init_cursor(mp, bma->tp, bma->ip, whichfork);
		bma->cur->bc_private.b.firstblock = *bma->firstblock;
		bma->cur->bc_private.b.dfops = bma->dfops;
	}
	/*
	 * Bump the number of extents we've allocated
	 * in this call.
	 */
	bma->nallocs++;

	if (bma->cur)
		bma->cur->bc_private.b.flags =
			bma->wasdel ? XFS_BTCUR_BPRV_WASDEL : 0;

	bma->got.br_startoff = bma->offset;
	bma->got.br_startblock = bma->blkno;
	bma->got.br_blockcount = bma->length;
	bma->got.br_state = XFS_EXT_NORM;

	/*
	 * In the data fork, a wasdelay extent has been initialized, so
	 * shouldn't be flagged as unwritten.
	 *
	 * For the cow fork, however, we convert delalloc reservations
	 * (extents allocated for speculative preallocation) to
	 * allocated unwritten extents, and only convert the unwritten
	 * extents to real extents when we're about to write the data.
	 */
	if ((!bma->wasdel || (bma->flags & XFS_BMAPI_COWFORK)) &&
	    (bma->flags & XFS_BMAPI_PREALLOC) &&
	    xfs_sb_version_hasextflgbit(&mp->m_sb))
		bma->got.br_state = XFS_EXT_UNWRITTEN;

	if (bma->wasdel)
		error = xfs_bmap_add_extent_delay_real(bma, whichfork);
	else
		error = xfs_bmap_add_extent_hole_real(bma->tp, bma->ip,
				whichfork, &bma->icur, &bma->cur, &bma->got,
				bma->firstblock, bma->dfops, &bma->logflags);

	bma->logflags |= tmp_logflags;
	if (error)
		return error;

	/*
	 * Update our extent pointer, given that xfs_bmap_add_extent_delay_real
	 * or xfs_bmap_add_extent_hole_real might have merged it into one of
	 * the neighbouring ones.
	 */
	xfs_iext_get_extent(ifp, &bma->icur, &bma->got);

	ASSERT(bma->got.br_startoff <= bma->offset);
	ASSERT(bma->got.br_startoff + bma->got.br_blockcount >=
	       bma->offset + bma->length);
	ASSERT(bma->got.br_state == XFS_EXT_NORM ||
	       bma->got.br_state == XFS_EXT_UNWRITTEN);
	return 0;
}