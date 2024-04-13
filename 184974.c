xfs_attr3_leaf_figure_balance(
	struct xfs_da_state		*state,
	struct xfs_da_state_blk		*blk1,
	struct xfs_attr3_icleaf_hdr	*ichdr1,
	struct xfs_da_state_blk		*blk2,
	struct xfs_attr3_icleaf_hdr	*ichdr2,
	int				*countarg,
	int				*usedbytesarg)
{
	struct xfs_attr_leafblock	*leaf1 = blk1->bp->b_addr;
	struct xfs_attr_leafblock	*leaf2 = blk2->bp->b_addr;
	struct xfs_attr_leaf_entry	*entry;
	int				count;
	int				max;
	int				index;
	int				totallen = 0;
	int				half;
	int				lastdelta;
	int				foundit = 0;
	int				tmp;

	/*
	 * Examine entries until we reduce the absolute difference in
	 * byte usage between the two blocks to a minimum.
	 */
	max = ichdr1->count + ichdr2->count;
	half = (max + 1) * sizeof(*entry);
	half += ichdr1->usedbytes + ichdr2->usedbytes +
			xfs_attr_leaf_newentsize(state->args->namelen,
						 state->args->valuelen,
						 state->blocksize, NULL);
	half /= 2;
	lastdelta = state->blocksize;
	entry = xfs_attr3_leaf_entryp(leaf1);
	for (count = index = 0; count < max; entry++, index++, count++) {

#define XFS_ATTR_ABS(A)	(((A) < 0) ? -(A) : (A))
		/*
		 * The new entry is in the first block, account for it.
		 */
		if (count == blk1->index) {
			tmp = totallen + sizeof(*entry) +
				xfs_attr_leaf_newentsize(
						state->args->namelen,
						state->args->valuelen,
						state->blocksize, NULL);
			if (XFS_ATTR_ABS(half - tmp) > lastdelta)
				break;
			lastdelta = XFS_ATTR_ABS(half - tmp);
			totallen = tmp;
			foundit = 1;
		}

		/*
		 * Wrap around into the second block if necessary.
		 */
		if (count == ichdr1->count) {
			leaf1 = leaf2;
			entry = xfs_attr3_leaf_entryp(leaf1);
			index = 0;
		}

		/*
		 * Figure out if next leaf entry would be too much.
		 */
		tmp = totallen + sizeof(*entry) + xfs_attr_leaf_entsize(leaf1,
									index);
		if (XFS_ATTR_ABS(half - tmp) > lastdelta)
			break;
		lastdelta = XFS_ATTR_ABS(half - tmp);
		totallen = tmp;
#undef XFS_ATTR_ABS
	}

	/*
	 * Calculate the number of usedbytes that will end up in lower block.
	 * If new entry not in lower block, fix up the count.
	 */
	totallen -= count * sizeof(*entry);
	if (foundit) {
		totallen -= sizeof(*entry) +
				xfs_attr_leaf_newentsize(
						state->args->namelen,
						state->args->valuelen,
						state->blocksize, NULL);
	}

	*countarg = count;
	*usedbytesarg = totallen;
	return foundit;
}