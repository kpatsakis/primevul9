static inline int should_cow_block(struct btrfs_trans_handle *trans,
				   struct btrfs_root *root,
				   struct extent_buffer *buf)
{
	if (btrfs_is_testing(root->fs_info))
		return 0;

	/* Ensure we can see the FORCE_COW bit */
	smp_mb__before_atomic();

	/*
	 * We do not need to cow a block if
	 * 1) this block is not created or changed in this transaction;
	 * 2) this block does not belong to TREE_RELOC tree;
	 * 3) the root is not forced COW.
	 *
	 * What is forced COW:
	 *    when we create snapshot during committing the transaction,
	 *    after we've finished copying src root, we must COW the shared
	 *    block to ensure the metadata consistency.
	 */
	if (btrfs_header_generation(buf) == trans->transid &&
	    !btrfs_header_flag(buf, BTRFS_HEADER_FLAG_WRITTEN) &&
	    !(root->root_key.objectid != BTRFS_TREE_RELOC_OBJECTID &&
	      btrfs_header_flag(buf, BTRFS_HEADER_FLAG_RELOC)) &&
	    !test_bit(BTRFS_ROOT_FORCE_COW, &root->state))
		return 0;
	return 1;
}