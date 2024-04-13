xfs_iunlink_map_ino(
	struct xfs_trans	*tp,
	xfs_agnumber_t		agno,
	xfs_agino_t		agino,
	struct xfs_imap		*imap,
	struct xfs_dinode	**dipp,
	struct xfs_buf		**bpp)
{
	struct xfs_mount	*mp = tp->t_mountp;
	int			error;

	imap->im_blkno = 0;
	error = xfs_imap(mp, tp, XFS_AGINO_TO_INO(mp, agno, agino), imap, 0);
	if (error) {
		xfs_warn(mp, "%s: xfs_imap returned error %d.",
				__func__, error);
		return error;
	}

	error = xfs_imap_to_bp(mp, tp, imap, dipp, bpp, 0);
	if (error) {
		xfs_warn(mp, "%s: xfs_imap_to_bp returned error %d.",
				__func__, error);
		return error;
	}

	return 0;
}