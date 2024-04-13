xfs_attr3_leaf_remove(
	struct xfs_buf		*bp,
	struct xfs_da_args	*args)
{
	struct xfs_attr_leafblock *leaf;
	struct xfs_attr3_icleaf_hdr ichdr;
	struct xfs_attr_leaf_entry *entry;
	struct xfs_mount	*mp = args->trans->t_mountp;
	int			before;
	int			after;
	int			smallest;
	int			entsize;
	int			tablesize;
	int			tmp;
	int			i;

	trace_xfs_attr_leaf_remove(args);

	leaf = bp->b_addr;
	xfs_attr3_leaf_hdr_from_disk(&ichdr, leaf);

	ASSERT(ichdr.count > 0 && ichdr.count < XFS_LBSIZE(mp) / 8);
	ASSERT(args->index >= 0 && args->index < ichdr.count);
	ASSERT(ichdr.firstused >= ichdr.count * sizeof(*entry) +
					xfs_attr3_leaf_hdr_size(leaf));

	entry = &xfs_attr3_leaf_entryp(leaf)[args->index];

	ASSERT(be16_to_cpu(entry->nameidx) >= ichdr.firstused);
	ASSERT(be16_to_cpu(entry->nameidx) < XFS_LBSIZE(mp));

	/*
	 * Scan through free region table:
	 *    check for adjacency of free'd entry with an existing one,
	 *    find smallest free region in case we need to replace it,
	 *    adjust any map that borders the entry table,
	 */
	tablesize = ichdr.count * sizeof(xfs_attr_leaf_entry_t)
					+ xfs_attr3_leaf_hdr_size(leaf);
	tmp = ichdr.freemap[0].size;
	before = after = -1;
	smallest = XFS_ATTR_LEAF_MAPSIZE - 1;
	entsize = xfs_attr_leaf_entsize(leaf, args->index);
	for (i = 0; i < XFS_ATTR_LEAF_MAPSIZE; i++) {
		ASSERT(ichdr.freemap[i].base < XFS_LBSIZE(mp));
		ASSERT(ichdr.freemap[i].size < XFS_LBSIZE(mp));
		if (ichdr.freemap[i].base == tablesize) {
			ichdr.freemap[i].base -= sizeof(xfs_attr_leaf_entry_t);
			ichdr.freemap[i].size += sizeof(xfs_attr_leaf_entry_t);
		}

		if (ichdr.freemap[i].base + ichdr.freemap[i].size ==
				be16_to_cpu(entry->nameidx)) {
			before = i;
		} else if (ichdr.freemap[i].base ==
				(be16_to_cpu(entry->nameidx) + entsize)) {
			after = i;
		} else if (ichdr.freemap[i].size < tmp) {
			tmp = ichdr.freemap[i].size;
			smallest = i;
		}
	}

	/*
	 * Coalesce adjacent freemap regions,
	 * or replace the smallest region.
	 */
	if ((before >= 0) || (after >= 0)) {
		if ((before >= 0) && (after >= 0)) {
			ichdr.freemap[before].size += entsize;
			ichdr.freemap[before].size += ichdr.freemap[after].size;
			ichdr.freemap[after].base = 0;
			ichdr.freemap[after].size = 0;
		} else if (before >= 0) {
			ichdr.freemap[before].size += entsize;
		} else {
			ichdr.freemap[after].base = be16_to_cpu(entry->nameidx);
			ichdr.freemap[after].size += entsize;
		}
	} else {
		/*
		 * Replace smallest region (if it is smaller than free'd entry)
		 */
		if (ichdr.freemap[smallest].size < entsize) {
			ichdr.freemap[smallest].base = be16_to_cpu(entry->nameidx);
			ichdr.freemap[smallest].size = entsize;
		}
	}

	/*
	 * Did we remove the first entry?
	 */
	if (be16_to_cpu(entry->nameidx) == ichdr.firstused)
		smallest = 1;
	else
		smallest = 0;

	/*
	 * Compress the remaining entries and zero out the removed stuff.
	 */
	memset(xfs_attr3_leaf_name(leaf, args->index), 0, entsize);
	ichdr.usedbytes -= entsize;
	xfs_trans_log_buf(args->trans, bp,
	     XFS_DA_LOGRANGE(leaf, xfs_attr3_leaf_name(leaf, args->index),
				   entsize));

	tmp = (ichdr.count - args->index) * sizeof(xfs_attr_leaf_entry_t);
	memmove(entry, entry + 1, tmp);
	ichdr.count--;
	xfs_trans_log_buf(args->trans, bp,
	    XFS_DA_LOGRANGE(leaf, entry, tmp + sizeof(xfs_attr_leaf_entry_t)));

	entry = &xfs_attr3_leaf_entryp(leaf)[ichdr.count];
	memset(entry, 0, sizeof(xfs_attr_leaf_entry_t));

	/*
	 * If we removed the first entry, re-find the first used byte
	 * in the name area.  Note that if the entry was the "firstused",
	 * then we don't have a "hole" in our block resulting from
	 * removing the name.
	 */
	if (smallest) {
		tmp = XFS_LBSIZE(mp);
		entry = xfs_attr3_leaf_entryp(leaf);
		for (i = ichdr.count - 1; i >= 0; entry++, i--) {
			ASSERT(be16_to_cpu(entry->nameidx) >= ichdr.firstused);
			ASSERT(be16_to_cpu(entry->nameidx) < XFS_LBSIZE(mp));

			if (be16_to_cpu(entry->nameidx) < tmp)
				tmp = be16_to_cpu(entry->nameidx);
		}
		ichdr.firstused = tmp;
		if (!ichdr.firstused)
			ichdr.firstused = tmp - XFS_ATTR_LEAF_NAME_ALIGN;
	} else {
		ichdr.holes = 1;	/* mark as needing compaction */
	}
	xfs_attr3_leaf_hdr_to_disk(leaf, &ichdr);
	xfs_trans_log_buf(args->trans, bp,
			  XFS_DA_LOGRANGE(leaf, &leaf->hdr,
					  xfs_attr3_leaf_hdr_size(leaf)));

	/*
	 * Check if leaf is less than 50% full, caller may want to
	 * "join" the leaf with a sibling if so.
	 */
	tmp = ichdr.usedbytes + xfs_attr3_leaf_hdr_size(leaf) +
	      ichdr.count * sizeof(xfs_attr_leaf_entry_t);

	return tmp < mp->m_attr_magicpct; /* leaf is < 37% full */
}