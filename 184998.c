xfs_attr3_leaf_create(
	struct xfs_da_args	*args,
	xfs_dablk_t		blkno,
	struct xfs_buf		**bpp)
{
	struct xfs_attr_leafblock *leaf;
	struct xfs_attr3_icleaf_hdr ichdr;
	struct xfs_inode	*dp = args->dp;
	struct xfs_mount	*mp = dp->i_mount;
	struct xfs_buf		*bp;
	int			error;

	trace_xfs_attr_leaf_create(args);

	error = xfs_da_get_buf(args->trans, args->dp, blkno, -1, &bp,
					    XFS_ATTR_FORK);
	if (error)
		return error;
	bp->b_ops = &xfs_attr3_leaf_buf_ops;
	xfs_trans_buf_set_type(args->trans, bp, XFS_BLFT_ATTR_LEAF_BUF);
	leaf = bp->b_addr;
	memset(leaf, 0, XFS_LBSIZE(mp));

	memset(&ichdr, 0, sizeof(ichdr));
	ichdr.firstused = XFS_LBSIZE(mp);

	if (xfs_sb_version_hascrc(&mp->m_sb)) {
		struct xfs_da3_blkinfo *hdr3 = bp->b_addr;

		ichdr.magic = XFS_ATTR3_LEAF_MAGIC;

		hdr3->blkno = cpu_to_be64(bp->b_bn);
		hdr3->owner = cpu_to_be64(dp->i_ino);
		uuid_copy(&hdr3->uuid, &mp->m_sb.sb_uuid);

		ichdr.freemap[0].base = sizeof(struct xfs_attr3_leaf_hdr);
	} else {
		ichdr.magic = XFS_ATTR_LEAF_MAGIC;
		ichdr.freemap[0].base = sizeof(struct xfs_attr_leaf_hdr);
	}
	ichdr.freemap[0].size = ichdr.firstused - ichdr.freemap[0].base;

	xfs_attr3_leaf_hdr_to_disk(leaf, &ichdr);
	xfs_trans_log_buf(args->trans, bp, 0, XFS_LBSIZE(mp) - 1);

	*bpp = bp;
	return 0;
}