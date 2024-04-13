xfs_bmap_btalloc(
	struct xfs_bmalloca	*ap)	/* bmap alloc argument struct */
{
	xfs_mount_t	*mp;		/* mount point structure */
	xfs_alloctype_t	atype = 0;	/* type for allocation routines */
	xfs_extlen_t	align = 0;	/* minimum allocation alignment */
	xfs_agnumber_t	fb_agno;	/* ag number of ap->firstblock */
	xfs_agnumber_t	ag;
	xfs_alloc_arg_t	args;
	xfs_fileoff_t	orig_offset;
	xfs_extlen_t	orig_length;
	xfs_extlen_t	blen;
	xfs_extlen_t	nextminlen = 0;
	int		nullfb;		/* true if ap->firstblock isn't set */
	int		isaligned;
	int		tryagain;
	int		error;
	int		stripe_align;

	ASSERT(ap->length);
	orig_offset = ap->offset;
	orig_length = ap->length;

	mp = ap->ip->i_mount;

	/* stripe alignment for allocation is determined by mount parameters */
	stripe_align = 0;
	if (mp->m_swidth && (mp->m_flags & XFS_MOUNT_SWALLOC))
		stripe_align = mp->m_swidth;
	else if (mp->m_dalign)
		stripe_align = mp->m_dalign;

	if (ap->flags & XFS_BMAPI_COWFORK)
		align = xfs_get_cowextsz_hint(ap->ip);
	else if (xfs_alloc_is_userdata(ap->datatype))
		align = xfs_get_extsz_hint(ap->ip);
	if (align) {
		error = xfs_bmap_extsize_align(mp, &ap->got, &ap->prev,
						align, 0, ap->eof, 0, ap->conv,
						&ap->offset, &ap->length);
		ASSERT(!error);
		ASSERT(ap->length);
	}


	nullfb = *ap->firstblock == NULLFSBLOCK;
	fb_agno = nullfb ? NULLAGNUMBER : XFS_FSB_TO_AGNO(mp, *ap->firstblock);
	if (nullfb) {
		if (xfs_alloc_is_userdata(ap->datatype) &&
		    xfs_inode_is_filestream(ap->ip)) {
			ag = xfs_filestream_lookup_ag(ap->ip);
			ag = (ag != NULLAGNUMBER) ? ag : 0;
			ap->blkno = XFS_AGB_TO_FSB(mp, ag, 0);
		} else {
			ap->blkno = XFS_INO_TO_FSB(mp, ap->ip->i_ino);
		}
	} else
		ap->blkno = *ap->firstblock;

	xfs_bmap_adjacent(ap);

	/*
	 * If allowed, use ap->blkno; otherwise must use firstblock since
	 * it's in the right allocation group.
	 */
	if (nullfb || XFS_FSB_TO_AGNO(mp, ap->blkno) == fb_agno)
		;
	else
		ap->blkno = *ap->firstblock;
	/*
	 * Normal allocation, done through xfs_alloc_vextent.
	 */
	tryagain = isaligned = 0;
	memset(&args, 0, sizeof(args));
	args.tp = ap->tp;
	args.mp = mp;
	args.fsbno = ap->blkno;
	xfs_rmap_skip_owner_update(&args.oinfo);

	/* Trim the allocation back to the maximum an AG can fit. */
	args.maxlen = MIN(ap->length, mp->m_ag_max_usable);
	args.firstblock = *ap->firstblock;
	blen = 0;
	if (nullfb) {
		/*
		 * Search for an allocation group with a single extent large
		 * enough for the request.  If one isn't found, then adjust
		 * the minimum allocation size to the largest space found.
		 */
		if (xfs_alloc_is_userdata(ap->datatype) &&
		    xfs_inode_is_filestream(ap->ip))
			error = xfs_bmap_btalloc_filestreams(ap, &args, &blen);
		else
			error = xfs_bmap_btalloc_nullfb(ap, &args, &blen);
		if (error)
			return error;
	} else if (ap->dfops->dop_low) {
		if (xfs_inode_is_filestream(ap->ip))
			args.type = XFS_ALLOCTYPE_FIRST_AG;
		else
			args.type = XFS_ALLOCTYPE_START_BNO;
		args.total = args.minlen = ap->minlen;
	} else {
		args.type = XFS_ALLOCTYPE_NEAR_BNO;
		args.total = ap->total;
		args.minlen = ap->minlen;
	}
	/* apply extent size hints if obtained earlier */
	if (align) {
		args.prod = align;
		if ((args.mod = (xfs_extlen_t)do_mod(ap->offset, args.prod)))
			args.mod = (xfs_extlen_t)(args.prod - args.mod);
	} else if (mp->m_sb.sb_blocksize >= PAGE_SIZE) {
		args.prod = 1;
		args.mod = 0;
	} else {
		args.prod = PAGE_SIZE >> mp->m_sb.sb_blocklog;
		if ((args.mod = (xfs_extlen_t)(do_mod(ap->offset, args.prod))))
			args.mod = (xfs_extlen_t)(args.prod - args.mod);
	}
	/*
	 * If we are not low on available data blocks, and the
	 * underlying logical volume manager is a stripe, and
	 * the file offset is zero then try to allocate data
	 * blocks on stripe unit boundary.
	 * NOTE: ap->aeof is only set if the allocation length
	 * is >= the stripe unit and the allocation offset is
	 * at the end of file.
	 */
	if (!ap->dfops->dop_low && ap->aeof) {
		if (!ap->offset) {
			args.alignment = stripe_align;
			atype = args.type;
			isaligned = 1;
			/*
			 * Adjust for alignment
			 */
			if (blen > args.alignment && blen <= args.maxlen)
				args.minlen = blen - args.alignment;
			args.minalignslop = 0;
		} else {
			/*
			 * First try an exact bno allocation.
			 * If it fails then do a near or start bno
			 * allocation with alignment turned on.
			 */
			atype = args.type;
			tryagain = 1;
			args.type = XFS_ALLOCTYPE_THIS_BNO;
			args.alignment = 1;
			/*
			 * Compute the minlen+alignment for the
			 * next case.  Set slop so that the value
			 * of minlen+alignment+slop doesn't go up
			 * between the calls.
			 */
			if (blen > stripe_align && blen <= args.maxlen)
				nextminlen = blen - stripe_align;
			else
				nextminlen = args.minlen;
			if (nextminlen + stripe_align > args.minlen + 1)
				args.minalignslop =
					nextminlen + stripe_align -
					args.minlen - 1;
			else
				args.minalignslop = 0;
		}
	} else {
		args.alignment = 1;
		args.minalignslop = 0;
	}
	args.minleft = ap->minleft;
	args.wasdel = ap->wasdel;
	args.resv = XFS_AG_RESV_NONE;
	args.datatype = ap->datatype;
	if (ap->datatype & XFS_ALLOC_USERDATA_ZERO)
		args.ip = ap->ip;

	error = xfs_alloc_vextent(&args);
	if (error)
		return error;

	if (tryagain && args.fsbno == NULLFSBLOCK) {
		/*
		 * Exact allocation failed. Now try with alignment
		 * turned on.
		 */
		args.type = atype;
		args.fsbno = ap->blkno;
		args.alignment = stripe_align;
		args.minlen = nextminlen;
		args.minalignslop = 0;
		isaligned = 1;
		if ((error = xfs_alloc_vextent(&args)))
			return error;
	}
	if (isaligned && args.fsbno == NULLFSBLOCK) {
		/*
		 * allocation failed, so turn off alignment and
		 * try again.
		 */
		args.type = atype;
		args.fsbno = ap->blkno;
		args.alignment = 0;
		if ((error = xfs_alloc_vextent(&args)))
			return error;
	}
	if (args.fsbno == NULLFSBLOCK && nullfb &&
	    args.minlen > ap->minlen) {
		args.minlen = ap->minlen;
		args.type = XFS_ALLOCTYPE_START_BNO;
		args.fsbno = ap->blkno;
		if ((error = xfs_alloc_vextent(&args)))
			return error;
	}
	if (args.fsbno == NULLFSBLOCK && nullfb) {
		args.fsbno = 0;
		args.type = XFS_ALLOCTYPE_FIRST_AG;
		args.total = ap->minlen;
		if ((error = xfs_alloc_vextent(&args)))
			return error;
		ap->dfops->dop_low = true;
	}
	if (args.fsbno != NULLFSBLOCK) {
		/*
		 * check the allocation happened at the same or higher AG than
		 * the first block that was allocated.
		 */
		ASSERT(*ap->firstblock == NULLFSBLOCK ||
		       XFS_FSB_TO_AGNO(mp, *ap->firstblock) <=
		       XFS_FSB_TO_AGNO(mp, args.fsbno));

		ap->blkno = args.fsbno;
		if (*ap->firstblock == NULLFSBLOCK)
			*ap->firstblock = args.fsbno;
		ASSERT(nullfb || fb_agno <= args.agno);
		ap->length = args.len;
		/*
		 * If the extent size hint is active, we tried to round the
		 * caller's allocation request offset down to extsz and the
		 * length up to another extsz boundary.  If we found a free
		 * extent we mapped it in starting at this new offset.  If the
		 * newly mapped space isn't long enough to cover any of the
		 * range of offsets that was originally requested, move the
		 * mapping up so that we can fill as much of the caller's
		 * original request as possible.  Free space is apparently
		 * very fragmented so we're unlikely to be able to satisfy the
		 * hints anyway.
		 */
		if (ap->length <= orig_length)
			ap->offset = orig_offset;
		else if (ap->offset + ap->length < orig_offset + orig_length)
			ap->offset = orig_offset + orig_length - ap->length;
		xfs_bmap_btalloc_accounting(ap, &args);
	} else {
		ap->blkno = NULLFSBLOCK;
		ap->length = 0;
	}
	return 0;
}