ext4_xattr_block_set(handle_t *handle, struct inode *inode,
		     struct ext4_xattr_info *i,
		     struct ext4_xattr_block_find *bs)
{
	struct super_block *sb = inode->i_sb;
	struct buffer_head *new_bh = NULL;
	struct ext4_xattr_search s_copy = bs->s;
	struct ext4_xattr_search *s = &s_copy;
	struct mb_cache_entry *ce = NULL;
	int error = 0;
	struct mb_cache *ea_block_cache = EA_BLOCK_CACHE(inode);
	struct inode *ea_inode = NULL, *tmp_inode;
	size_t old_ea_inode_quota = 0;
	unsigned int ea_ino;


#define header(x) ((struct ext4_xattr_header *)(x))

	if (s->base) {
		BUFFER_TRACE(bs->bh, "get_write_access");
		error = ext4_journal_get_write_access(handle, bs->bh);
		if (error)
			goto cleanup;
		lock_buffer(bs->bh);

		if (header(s->base)->h_refcount == cpu_to_le32(1)) {
			__u32 hash = le32_to_cpu(BHDR(bs->bh)->h_hash);

			/*
			 * This must happen under buffer lock for
			 * ext4_xattr_block_set() to reliably detect modified
			 * block
			 */
			if (ea_block_cache)
				mb_cache_entry_delete(ea_block_cache, hash,
						      bs->bh->b_blocknr);
			ea_bdebug(bs->bh, "modifying in-place");
			error = ext4_xattr_set_entry(i, s, handle, inode,
						     true /* is_block */);
			ext4_xattr_block_csum_set(inode, bs->bh);
			unlock_buffer(bs->bh);
			if (error == -EFSCORRUPTED)
				goto bad_block;
			if (!error)
				error = ext4_handle_dirty_metadata(handle,
								   inode,
								   bs->bh);
			if (error)
				goto cleanup;
			goto inserted;
		} else {
			int offset = (char *)s->here - bs->bh->b_data;

			unlock_buffer(bs->bh);
			ea_bdebug(bs->bh, "cloning");
			s->base = kmalloc(bs->bh->b_size, GFP_NOFS);
			error = -ENOMEM;
			if (s->base == NULL)
				goto cleanup;
			memcpy(s->base, BHDR(bs->bh), bs->bh->b_size);
			s->first = ENTRY(header(s->base)+1);
			header(s->base)->h_refcount = cpu_to_le32(1);
			s->here = ENTRY(s->base + offset);
			s->end = s->base + bs->bh->b_size;

			/*
			 * If existing entry points to an xattr inode, we need
			 * to prevent ext4_xattr_set_entry() from decrementing
			 * ref count on it because the reference belongs to the
			 * original block. In this case, make the entry look
			 * like it has an empty value.
			 */
			if (!s->not_found && s->here->e_value_inum) {
				ea_ino = le32_to_cpu(s->here->e_value_inum);
				error = ext4_xattr_inode_iget(inode, ea_ino,
					      le32_to_cpu(s->here->e_hash),
					      &tmp_inode);
				if (error)
					goto cleanup;

				if (!ext4_test_inode_state(tmp_inode,
						EXT4_STATE_LUSTRE_EA_INODE)) {
					/*
					 * Defer quota free call for previous
					 * inode until success is guaranteed.
					 */
					old_ea_inode_quota = le32_to_cpu(
							s->here->e_value_size);
				}
				iput(tmp_inode);

				s->here->e_value_inum = 0;
				s->here->e_value_size = 0;
			}
		}
	} else {
		/* Allocate a buffer where we construct the new block. */
		s->base = kzalloc(sb->s_blocksize, GFP_NOFS);
		/* assert(header == s->base) */
		error = -ENOMEM;
		if (s->base == NULL)
			goto cleanup;
		header(s->base)->h_magic = cpu_to_le32(EXT4_XATTR_MAGIC);
		header(s->base)->h_blocks = cpu_to_le32(1);
		header(s->base)->h_refcount = cpu_to_le32(1);
		s->first = ENTRY(header(s->base)+1);
		s->here = ENTRY(header(s->base)+1);
		s->end = s->base + sb->s_blocksize;
	}

	error = ext4_xattr_set_entry(i, s, handle, inode, true /* is_block */);
	if (error == -EFSCORRUPTED)
		goto bad_block;
	if (error)
		goto cleanup;

	if (i->value && s->here->e_value_inum) {
		/*
		 * A ref count on ea_inode has been taken as part of the call to
		 * ext4_xattr_set_entry() above. We would like to drop this
		 * extra ref but we have to wait until the xattr block is
		 * initialized and has its own ref count on the ea_inode.
		 */
		ea_ino = le32_to_cpu(s->here->e_value_inum);
		error = ext4_xattr_inode_iget(inode, ea_ino,
					      le32_to_cpu(s->here->e_hash),
					      &ea_inode);
		if (error) {
			ea_inode = NULL;
			goto cleanup;
		}
	}

inserted:
	if (!IS_LAST_ENTRY(s->first)) {
		new_bh = ext4_xattr_block_cache_find(inode, header(s->base),
						     &ce);
		if (new_bh) {
			/* We found an identical block in the cache. */
			if (new_bh == bs->bh)
				ea_bdebug(new_bh, "keeping");
			else {
				u32 ref;

				WARN_ON_ONCE(dquot_initialize_needed(inode));

				/* The old block is released after updating
				   the inode. */
				error = dquot_alloc_block(inode,
						EXT4_C2B(EXT4_SB(sb), 1));
				if (error)
					goto cleanup;
				BUFFER_TRACE(new_bh, "get_write_access");
				error = ext4_journal_get_write_access(handle,
								      new_bh);
				if (error)
					goto cleanup_dquot;
				lock_buffer(new_bh);
				/*
				 * We have to be careful about races with
				 * freeing, rehashing or adding references to
				 * xattr block. Once we hold buffer lock xattr
				 * block's state is stable so we can check
				 * whether the block got freed / rehashed or
				 * not.  Since we unhash mbcache entry under
				 * buffer lock when freeing / rehashing xattr
				 * block, checking whether entry is still
				 * hashed is reliable. Same rules hold for
				 * e_reusable handling.
				 */
				if (hlist_bl_unhashed(&ce->e_hash_list) ||
				    !ce->e_reusable) {
					/*
					 * Undo everything and check mbcache
					 * again.
					 */
					unlock_buffer(new_bh);
					dquot_free_block(inode,
							 EXT4_C2B(EXT4_SB(sb),
								  1));
					brelse(new_bh);
					mb_cache_entry_put(ea_block_cache, ce);
					ce = NULL;
					new_bh = NULL;
					goto inserted;
				}
				ref = le32_to_cpu(BHDR(new_bh)->h_refcount) + 1;
				BHDR(new_bh)->h_refcount = cpu_to_le32(ref);
				if (ref >= EXT4_XATTR_REFCOUNT_MAX)
					ce->e_reusable = 0;
				ea_bdebug(new_bh, "reusing; refcount now=%d",
					  ref);
				ext4_xattr_block_csum_set(inode, new_bh);
				unlock_buffer(new_bh);
				error = ext4_handle_dirty_metadata(handle,
								   inode,
								   new_bh);
				if (error)
					goto cleanup_dquot;
			}
			mb_cache_entry_touch(ea_block_cache, ce);
			mb_cache_entry_put(ea_block_cache, ce);
			ce = NULL;
		} else if (bs->bh && s->base == bs->bh->b_data) {
			/* We were modifying this block in-place. */
			ea_bdebug(bs->bh, "keeping this block");
			ext4_xattr_block_cache_insert(ea_block_cache, bs->bh);
			new_bh = bs->bh;
			get_bh(new_bh);
		} else {
			/* We need to allocate a new block */
			ext4_fsblk_t goal, block;

			WARN_ON_ONCE(dquot_initialize_needed(inode));

			goal = ext4_group_first_block_no(sb,
						EXT4_I(inode)->i_block_group);

			/* non-extent files can't have physical blocks past 2^32 */
			if (!(ext4_test_inode_flag(inode, EXT4_INODE_EXTENTS)))
				goal = goal & EXT4_MAX_BLOCK_FILE_PHYS;

			block = ext4_new_meta_blocks(handle, inode, goal, 0,
						     NULL, &error);
			if (error)
				goto cleanup;

			if (!(ext4_test_inode_flag(inode, EXT4_INODE_EXTENTS)))
				BUG_ON(block > EXT4_MAX_BLOCK_FILE_PHYS);

			ea_idebug(inode, "creating block %llu",
				  (unsigned long long)block);

			new_bh = sb_getblk(sb, block);
			if (unlikely(!new_bh)) {
				error = -ENOMEM;
getblk_failed:
				ext4_free_blocks(handle, inode, NULL, block, 1,
						 EXT4_FREE_BLOCKS_METADATA);
				goto cleanup;
			}
			error = ext4_xattr_inode_inc_ref_all(handle, inode,
						      ENTRY(header(s->base)+1));
			if (error)
				goto getblk_failed;
			if (ea_inode) {
				/* Drop the extra ref on ea_inode. */
				error = ext4_xattr_inode_dec_ref(handle,
								 ea_inode);
				if (error)
					ext4_warning_inode(ea_inode,
							   "dec ref error=%d",
							   error);
				iput(ea_inode);
				ea_inode = NULL;
			}

			lock_buffer(new_bh);
			error = ext4_journal_get_create_access(handle, new_bh);
			if (error) {
				unlock_buffer(new_bh);
				error = -EIO;
				goto getblk_failed;
			}
			memcpy(new_bh->b_data, s->base, new_bh->b_size);
			ext4_xattr_block_csum_set(inode, new_bh);
			set_buffer_uptodate(new_bh);
			unlock_buffer(new_bh);
			ext4_xattr_block_cache_insert(ea_block_cache, new_bh);
			error = ext4_handle_dirty_metadata(handle, inode,
							   new_bh);
			if (error)
				goto cleanup;
		}
	}

	if (old_ea_inode_quota)
		ext4_xattr_inode_free_quota(inode, NULL, old_ea_inode_quota);

	/* Update the inode. */
	EXT4_I(inode)->i_file_acl = new_bh ? new_bh->b_blocknr : 0;

	/* Drop the previous xattr block. */
	if (bs->bh && bs->bh != new_bh) {
		struct ext4_xattr_inode_array *ea_inode_array = NULL;

		ext4_xattr_release_block(handle, inode, bs->bh,
					 &ea_inode_array,
					 0 /* extra_credits */);
		ext4_xattr_inode_array_free(ea_inode_array);
	}
	error = 0;

cleanup:
	if (ea_inode) {
		int error2;

		error2 = ext4_xattr_inode_dec_ref(handle, ea_inode);
		if (error2)
			ext4_warning_inode(ea_inode, "dec ref error=%d",
					   error2);

		/* If there was an error, revert the quota charge. */
		if (error)
			ext4_xattr_inode_free_quota(inode, ea_inode,
						    i_size_read(ea_inode));
		iput(ea_inode);
	}
	if (ce)
		mb_cache_entry_put(ea_block_cache, ce);
	brelse(new_bh);
	if (!(bs->bh && s->base == bs->bh->b_data))
		kfree(s->base);

	return error;

cleanup_dquot:
	dquot_free_block(inode, EXT4_C2B(EXT4_SB(sb), 1));
	goto cleanup;

bad_block:
	EXT4_ERROR_INODE(inode, "bad block %llu",
			 EXT4_I(inode)->i_file_acl);
	goto cleanup;

#undef header
}