xfs_attr3_leaf_read(
	struct xfs_trans	*tp,
	struct xfs_inode	*dp,
	xfs_dablk_t		bno,
	xfs_daddr_t		mappedbno,
	struct xfs_buf		**bpp)
{
	int			err;

	err = xfs_da_read_buf(tp, dp, bno, mappedbno, bpp,
				XFS_ATTR_FORK, &xfs_attr3_leaf_buf_ops);
	if (!err && tp)
		xfs_trans_buf_set_type(tp, *bpp, XFS_BLFT_ATTR_LEAF_BUF);
	return err;
}