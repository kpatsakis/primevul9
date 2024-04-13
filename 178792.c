xfs_bmapi_update_map(
	struct xfs_bmbt_irec	**map,
	xfs_fileoff_t		*bno,
	xfs_filblks_t		*len,
	xfs_fileoff_t		obno,
	xfs_fileoff_t		end,
	int			*n,
	int			flags)
{
	xfs_bmbt_irec_t	*mval = *map;

	ASSERT((flags & XFS_BMAPI_ENTIRE) ||
	       ((mval->br_startoff + mval->br_blockcount) <= end));
	ASSERT((flags & XFS_BMAPI_ENTIRE) || (mval->br_blockcount <= *len) ||
	       (mval->br_startoff < obno));

	*bno = mval->br_startoff + mval->br_blockcount;
	*len = end - *bno;
	if (*n > 0 && mval->br_startoff == mval[-1].br_startoff) {
		/* update previous map with new information */
		ASSERT(mval->br_startblock == mval[-1].br_startblock);
		ASSERT(mval->br_blockcount > mval[-1].br_blockcount);
		ASSERT(mval->br_state == mval[-1].br_state);
		mval[-1].br_blockcount = mval->br_blockcount;
		mval[-1].br_state = mval->br_state;
	} else if (*n > 0 && mval->br_startblock != DELAYSTARTBLOCK &&
		   mval[-1].br_startblock != DELAYSTARTBLOCK &&
		   mval[-1].br_startblock != HOLESTARTBLOCK &&
		   mval->br_startblock == mval[-1].br_startblock +
					  mval[-1].br_blockcount &&
		   ((flags & XFS_BMAPI_IGSTATE) ||
			mval[-1].br_state == mval->br_state)) {
		ASSERT(mval->br_startoff ==
		       mval[-1].br_startoff + mval[-1].br_blockcount);
		mval[-1].br_blockcount += mval->br_blockcount;
	} else if (*n > 0 &&
		   mval->br_startblock == DELAYSTARTBLOCK &&
		   mval[-1].br_startblock == DELAYSTARTBLOCK &&
		   mval->br_startoff ==
		   mval[-1].br_startoff + mval[-1].br_blockcount) {
		mval[-1].br_blockcount += mval->br_blockcount;
		mval[-1].br_state = mval->br_state;
	} else if (!((*n == 0) &&
		     ((mval->br_startoff + mval->br_blockcount) <=
		      obno))) {
		mval++;
		(*n)++;
	}
	*map = mval;
}