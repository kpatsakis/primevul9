xfs_trim_extent(
	struct xfs_bmbt_irec	*irec,
	xfs_fileoff_t		bno,
	xfs_filblks_t		len)
{
	xfs_fileoff_t		distance;
	xfs_fileoff_t		end = bno + len;

	if (irec->br_startoff + irec->br_blockcount <= bno ||
	    irec->br_startoff >= end) {
		irec->br_blockcount = 0;
		return;
	}

	if (irec->br_startoff < bno) {
		distance = bno - irec->br_startoff;
		if (isnullstartblock(irec->br_startblock))
			irec->br_startblock = DELAYSTARTBLOCK;
		if (irec->br_startblock != DELAYSTARTBLOCK &&
		    irec->br_startblock != HOLESTARTBLOCK)
			irec->br_startblock += distance;
		irec->br_startoff += distance;
		irec->br_blockcount -= distance;
	}

	if (end < irec->br_startoff + irec->br_blockcount) {
		distance = irec->br_startoff + irec->br_blockcount - end;
		irec->br_blockcount -= distance;
	}
}