xfs_iunlink_update_inode(
	struct xfs_trans	*tp,
	struct xfs_inode	*ip,
	xfs_agnumber_t		agno,
	xfs_agino_t		next_agino,
	xfs_agino_t		*old_next_agino)
{
	struct xfs_mount	*mp = tp->t_mountp;
	struct xfs_dinode	*dip;
	struct xfs_buf		*ibp;
	xfs_agino_t		old_value;
	int			error;

	ASSERT(xfs_verify_agino_or_null(mp, agno, next_agino));

	error = xfs_imap_to_bp(mp, tp, &ip->i_imap, &dip, &ibp, 0);
	if (error)
		return error;

	/* Make sure the old pointer isn't garbage. */
	old_value = be32_to_cpu(dip->di_next_unlinked);
	if (!xfs_verify_agino_or_null(mp, agno, old_value)) {
		xfs_inode_verifier_error(ip, -EFSCORRUPTED, __func__, dip,
				sizeof(*dip), __this_address);
		error = -EFSCORRUPTED;
		goto out;
	}

	/*
	 * Since we're updating a linked list, we should never find that the
	 * current pointer is the same as the new value, unless we're
	 * terminating the list.
	 */
	*old_next_agino = old_value;
	if (old_value == next_agino) {
		if (next_agino != NULLAGINO) {
			xfs_inode_verifier_error(ip, -EFSCORRUPTED, __func__,
					dip, sizeof(*dip), __this_address);
			error = -EFSCORRUPTED;
		}
		goto out;
	}

	/* Ok, update the new pointer. */
	xfs_iunlink_update_dinode(tp, agno, XFS_INO_TO_AGINO(mp, ip->i_ino),
			ibp, dip, &ip->i_imap, next_agino);
	return 0;
out:
	xfs_trans_brelse(tp, ibp);
	return error;
}