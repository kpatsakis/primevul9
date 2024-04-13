xfs_attr3_leaf_unbalance(
	struct xfs_da_state	*state,
	struct xfs_da_state_blk	*drop_blk,
	struct xfs_da_state_blk	*save_blk)
{
	struct xfs_attr_leafblock *drop_leaf = drop_blk->bp->b_addr;
	struct xfs_attr_leafblock *save_leaf = save_blk->bp->b_addr;
	struct xfs_attr3_icleaf_hdr drophdr;
	struct xfs_attr3_icleaf_hdr savehdr;
	struct xfs_attr_leaf_entry *entry;
	struct xfs_mount	*mp = state->mp;

	trace_xfs_attr_leaf_unbalance(state->args);

	drop_leaf = drop_blk->bp->b_addr;
	save_leaf = save_blk->bp->b_addr;
	xfs_attr3_leaf_hdr_from_disk(&drophdr, drop_leaf);
	xfs_attr3_leaf_hdr_from_disk(&savehdr, save_leaf);
	entry = xfs_attr3_leaf_entryp(drop_leaf);

	/*
	 * Save last hashval from dying block for later Btree fixup.
	 */
	drop_blk->hashval = be32_to_cpu(entry[drophdr.count - 1].hashval);

	/*
	 * Check if we need a temp buffer, or can we do it in place.
	 * Note that we don't check "leaf" for holes because we will
	 * always be dropping it, toosmall() decided that for us already.
	 */
	if (savehdr.holes == 0) {
		/*
		 * dest leaf has no holes, so we add there.  May need
		 * to make some room in the entry array.
		 */
		if (xfs_attr3_leaf_order(save_blk->bp, &savehdr,
					 drop_blk->bp, &drophdr)) {
			xfs_attr3_leaf_moveents(drop_leaf, &drophdr, 0,
						save_leaf, &savehdr, 0,
						drophdr.count, mp);
		} else {
			xfs_attr3_leaf_moveents(drop_leaf, &drophdr, 0,
						save_leaf, &savehdr,
						savehdr.count, drophdr.count, mp);
		}
	} else {
		/*
		 * Destination has holes, so we make a temporary copy
		 * of the leaf and add them both to that.
		 */
		struct xfs_attr_leafblock *tmp_leaf;
		struct xfs_attr3_icleaf_hdr tmphdr;

		tmp_leaf = kmem_alloc(state->blocksize, KM_SLEEP);
		memset(tmp_leaf, 0, state->blocksize);
		memset(&tmphdr, 0, sizeof(tmphdr));

		tmphdr.magic = savehdr.magic;
		tmphdr.forw = savehdr.forw;
		tmphdr.back = savehdr.back;
		tmphdr.firstused = state->blocksize;
		if (xfs_attr3_leaf_order(save_blk->bp, &savehdr,
					 drop_blk->bp, &drophdr)) {
			xfs_attr3_leaf_moveents(drop_leaf, &drophdr, 0,
						tmp_leaf, &tmphdr, 0,
						drophdr.count, mp);
			xfs_attr3_leaf_moveents(save_leaf, &savehdr, 0,
						tmp_leaf, &tmphdr, tmphdr.count,
						savehdr.count, mp);
		} else {
			xfs_attr3_leaf_moveents(save_leaf, &savehdr, 0,
						tmp_leaf, &tmphdr, 0,
						savehdr.count, mp);
			xfs_attr3_leaf_moveents(drop_leaf, &drophdr, 0,
						tmp_leaf, &tmphdr, tmphdr.count,
						drophdr.count, mp);
		}
		memcpy(save_leaf, tmp_leaf, state->blocksize);
		savehdr = tmphdr; /* struct copy */
		kmem_free(tmp_leaf);
	}

	xfs_attr3_leaf_hdr_to_disk(save_leaf, &savehdr);
	xfs_trans_log_buf(state->args->trans, save_blk->bp, 0,
					   state->blocksize - 1);

	/*
	 * Copy out last hashval in each block for B-tree code.
	 */
	entry = xfs_attr3_leaf_entryp(save_leaf);
	save_blk->hashval = be32_to_cpu(entry[savehdr.count - 1].hashval);
}