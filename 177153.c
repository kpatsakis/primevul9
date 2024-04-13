static int ext4_xattr_inode_update_ref(handle_t *handle, struct inode *ea_inode,
				       int ref_change)
{
	struct mb_cache *ea_inode_cache = EA_INODE_CACHE(ea_inode);
	struct ext4_iloc iloc;
	s64 ref_count;
	u32 hash;
	int ret;

	inode_lock(ea_inode);

	ret = ext4_reserve_inode_write(handle, ea_inode, &iloc);
	if (ret) {
		iloc.bh = NULL;
		goto out;
	}

	ref_count = ext4_xattr_inode_get_ref(ea_inode);
	ref_count += ref_change;
	ext4_xattr_inode_set_ref(ea_inode, ref_count);

	if (ref_change > 0) {
		WARN_ONCE(ref_count <= 0, "EA inode %lu ref_count=%lld",
			  ea_inode->i_ino, ref_count);

		if (ref_count == 1) {
			WARN_ONCE(ea_inode->i_nlink, "EA inode %lu i_nlink=%u",
				  ea_inode->i_ino, ea_inode->i_nlink);

			set_nlink(ea_inode, 1);
			ext4_orphan_del(handle, ea_inode);

			if (ea_inode_cache) {
				hash = ext4_xattr_inode_get_hash(ea_inode);
				mb_cache_entry_create(ea_inode_cache,
						      GFP_NOFS, hash,
						      ea_inode->i_ino,
						      true /* reusable */);
			}
		}
	} else {
		WARN_ONCE(ref_count < 0, "EA inode %lu ref_count=%lld",
			  ea_inode->i_ino, ref_count);

		if (ref_count == 0) {
			WARN_ONCE(ea_inode->i_nlink != 1,
				  "EA inode %lu i_nlink=%u",
				  ea_inode->i_ino, ea_inode->i_nlink);

			clear_nlink(ea_inode);
			ext4_orphan_add(handle, ea_inode);

			if (ea_inode_cache) {
				hash = ext4_xattr_inode_get_hash(ea_inode);
				mb_cache_entry_delete(ea_inode_cache, hash,
						      ea_inode->i_ino);
			}
		}
	}

	ret = ext4_mark_iloc_dirty(handle, ea_inode, &iloc);
	iloc.bh = NULL;
	if (ret)
		ext4_warning_inode(ea_inode,
				   "ext4_mark_iloc_dirty() failed ret=%d", ret);
out:
	brelse(iloc.bh);
	inode_unlock(ea_inode);
	return ret;
}