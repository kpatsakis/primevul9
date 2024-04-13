xfs_iunlink_update_dinode(
	struct xfs_trans	*tp,
	xfs_agnumber_t		agno,
	xfs_agino_t		agino,
	struct xfs_buf		*ibp,
	struct xfs_dinode	*dip,
	struct xfs_imap		*imap,
	xfs_agino_t		next_agino)
{
	struct xfs_mount	*mp = tp->t_mountp;
	int			offset;

	ASSERT(xfs_verify_agino_or_null(mp, agno, next_agino));

	trace_xfs_iunlink_update_dinode(mp, agno, agino,
			be32_to_cpu(dip->di_next_unlinked), next_agino);

	dip->di_next_unlinked = cpu_to_be32(next_agino);
	offset = imap->im_boffset +
			offsetof(struct xfs_dinode, di_next_unlinked);

	/* need to recalc the inode CRC if appropriate */
	xfs_dinode_calc_crc(mp, dip);
	xfs_trans_inode_buf(tp, ibp);
	xfs_trans_log_buf(tp, ibp, offset, offset + sizeof(xfs_agino_t) - 1);
}