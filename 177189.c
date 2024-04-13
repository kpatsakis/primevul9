ext4_xattr_release_block(handle_t *handle, struct inode *inode,
			 struct buffer_head *bh,
			 struct ext4_xattr_inode_array **ea_inode_array,
			 int extra_credits)
{
	struct mb_cache *ea_block_cache = EA_BLOCK_CACHE(inode);
	u32 hash, ref;
	int error = 0;

	BUFFER_TRACE(bh, "get_write_access");
	error = ext4_journal_get_write_access(handle, bh);
	if (error)
		goto out;

	lock_buffer(bh);
	hash = le32_to_cpu(BHDR(bh)->h_hash);
	ref = le32_to_cpu(BHDR(bh)->h_refcount);
	if (ref == 1) {
		ea_bdebug(bh, "refcount now=0; freeing");
		/*
		 * This must happen under buffer lock for
		 * ext4_xattr_block_set() to reliably detect freed block
		 */
		if (ea_block_cache)
			mb_cache_entry_delete(ea_block_cache, hash,
					      bh->b_blocknr);
		get_bh(bh);
		unlock_buffer(bh);

		if (ext4_has_feature_ea_inode(inode->i_sb))
			ext4_xattr_inode_dec_ref_all(handle, inode, bh,
						     BFIRST(bh),
						     true /* block_csum */,
						     ea_inode_array,
						     extra_credits,
						     true /* skip_quota */);
		ext4_free_blocks(handle, inode, bh, 0, 1,
				 EXT4_FREE_BLOCKS_METADATA |
				 EXT4_FREE_BLOCKS_FORGET);
	} else {
		ref--;
		BHDR(bh)->h_refcount = cpu_to_le32(ref);
		if (ref == EXT4_XATTR_REFCOUNT_MAX - 1) {
			struct mb_cache_entry *ce;

			if (ea_block_cache) {
				ce = mb_cache_entry_get(ea_block_cache, hash,
							bh->b_blocknr);
				if (ce) {
					ce->e_reusable = 1;
					mb_cache_entry_put(ea_block_cache, ce);
				}
			}
		}

		ext4_xattr_block_csum_set(inode, bh);
		/*
		 * Beware of this ugliness: Releasing of xattr block references
		 * from different inodes can race and so we have to protect
		 * from a race where someone else frees the block (and releases
		 * its journal_head) before we are done dirtying the buffer. In
		 * nojournal mode this race is harmless and we actually cannot
		 * call ext4_handle_dirty_metadata() with locked buffer as
		 * that function can call sync_dirty_buffer() so for that case
		 * we handle the dirtying after unlocking the buffer.
		 */
		if (ext4_handle_valid(handle))
			error = ext4_handle_dirty_metadata(handle, inode, bh);
		unlock_buffer(bh);
		if (!ext4_handle_valid(handle))
			error = ext4_handle_dirty_metadata(handle, inode, bh);
		if (IS_SYNC(inode))
			ext4_handle_sync(handle);
		dquot_free_block(inode, EXT4_C2B(EXT4_SB(inode->i_sb), 1));
		ea_bdebug(bh, "refcount now=%d; releasing",
			  le32_to_cpu(BHDR(bh)->h_refcount));
	}
out:
	ext4_std_error(inode->i_sb, error);
	return;
}