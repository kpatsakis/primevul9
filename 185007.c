xfs_attr3_leaf_compact(
	struct xfs_da_args	*args,
	struct xfs_attr3_icleaf_hdr *ichdr_d,
	struct xfs_buf		*bp)
{
	xfs_attr_leafblock_t	*leaf_s, *leaf_d;
	struct xfs_attr3_icleaf_hdr ichdr_s;
	struct xfs_trans	*trans = args->trans;
	struct xfs_mount	*mp = trans->t_mountp;
	char			*tmpbuffer;

	trace_xfs_attr_leaf_compact(args);

	tmpbuffer = kmem_alloc(XFS_LBSIZE(mp), KM_SLEEP);
	ASSERT(tmpbuffer != NULL);
	memcpy(tmpbuffer, bp->b_addr, XFS_LBSIZE(mp));
	memset(bp->b_addr, 0, XFS_LBSIZE(mp));

	/*
	 * Copy basic information
	 */
	leaf_s = (xfs_attr_leafblock_t *)tmpbuffer;
	leaf_d = bp->b_addr;
	ichdr_s = *ichdr_d;	/* struct copy */
	ichdr_d->firstused = XFS_LBSIZE(mp);
	ichdr_d->usedbytes = 0;
	ichdr_d->count = 0;
	ichdr_d->holes = 0;
	ichdr_d->freemap[0].base = xfs_attr3_leaf_hdr_size(leaf_s);
	ichdr_d->freemap[0].size = ichdr_d->firstused - ichdr_d->freemap[0].base;

	/*
	 * Copy all entry's in the same (sorted) order,
	 * but allocate name/value pairs packed and in sequence.
	 */
	xfs_attr3_leaf_moveents(leaf_s, &ichdr_s, 0, leaf_d, ichdr_d, 0,
				ichdr_s.count, mp);
	/*
	 * this logs the entire buffer, but the caller must write the header
	 * back to the buffer when it is finished modifying it.
	 */
	xfs_trans_log_buf(trans, bp, 0, XFS_LBSIZE(mp) - 1);

	kmem_free(tmpbuffer);
}