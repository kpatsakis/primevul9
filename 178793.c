xfs_bmap_add_attrfork_extents(
	xfs_trans_t		*tp,		/* transaction pointer */
	xfs_inode_t		*ip,		/* incore inode pointer */
	xfs_fsblock_t		*firstblock,	/* first block allocated */
	struct xfs_defer_ops	*dfops,		/* blocks to free at commit */
	int			*flags)		/* inode logging flags */
{
	xfs_btree_cur_t		*cur;		/* bmap btree cursor */
	int			error;		/* error return value */

	if (ip->i_d.di_nextents * sizeof(xfs_bmbt_rec_t) <= XFS_IFORK_DSIZE(ip))
		return 0;
	cur = NULL;
	error = xfs_bmap_extents_to_btree(tp, ip, firstblock, dfops, &cur, 0,
		flags, XFS_DATA_FORK);
	if (cur) {
		cur->bc_private.b.allocated = 0;
		xfs_btree_del_cursor(cur,
			error ? XFS_BTREE_ERROR : XFS_BTREE_NOERROR);
	}
	return error;
}