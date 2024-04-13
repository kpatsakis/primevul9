xfs_attr3_leaf_split(
	struct xfs_da_state	*state,
	struct xfs_da_state_blk	*oldblk,
	struct xfs_da_state_blk	*newblk)
{
	xfs_dablk_t blkno;
	int error;

	trace_xfs_attr_leaf_split(state->args);

	/*
	 * Allocate space for a new leaf node.
	 */
	ASSERT(oldblk->magic == XFS_ATTR_LEAF_MAGIC);
	error = xfs_da_grow_inode(state->args, &blkno);
	if (error)
		return(error);
	error = xfs_attr3_leaf_create(state->args, blkno, &newblk->bp);
	if (error)
		return(error);
	newblk->blkno = blkno;
	newblk->magic = XFS_ATTR_LEAF_MAGIC;

	/*
	 * Rebalance the entries across the two leaves.
	 * NOTE: rebalance() currently depends on the 2nd block being empty.
	 */
	xfs_attr3_leaf_rebalance(state, oldblk, newblk);
	error = xfs_da3_blk_link(state, oldblk, newblk);
	if (error)
		return(error);

	/*
	 * Save info on "old" attribute for "atomic rename" ops, leaf_add()
	 * modifies the index/blkno/rmtblk/rmtblkcnt fields to show the
	 * "new" attrs info.  Will need the "old" info to remove it later.
	 *
	 * Insert the "new" entry in the correct block.
	 */
	if (state->inleaf) {
		trace_xfs_attr_leaf_add_old(state->args);
		error = xfs_attr3_leaf_add(oldblk->bp, state->args);
	} else {
		trace_xfs_attr_leaf_add_new(state->args);
		error = xfs_attr3_leaf_add(newblk->bp, state->args);
	}

	/*
	 * Update last hashval in each block since we added the name.
	 */
	oldblk->hashval = xfs_attr_leaf_lasthash(oldblk->bp, NULL);
	newblk->hashval = xfs_attr_leaf_lasthash(newblk->bp, NULL);
	return(error);
}