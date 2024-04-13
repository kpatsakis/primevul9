ext4_xattr_inode_cache_find(struct inode *inode, const void *value,
			    size_t value_len, u32 hash)
{
	struct inode *ea_inode;
	struct mb_cache_entry *ce;
	struct mb_cache *ea_inode_cache = EA_INODE_CACHE(inode);
	void *ea_data;

	if (!ea_inode_cache)
		return NULL;

	ce = mb_cache_entry_find_first(ea_inode_cache, hash);
	if (!ce)
		return NULL;

	ea_data = ext4_kvmalloc(value_len, GFP_NOFS);
	if (!ea_data) {
		mb_cache_entry_put(ea_inode_cache, ce);
		return NULL;
	}

	while (ce) {
		ea_inode = ext4_iget(inode->i_sb, ce->e_value);
		if (!IS_ERR(ea_inode) &&
		    !is_bad_inode(ea_inode) &&
		    (EXT4_I(ea_inode)->i_flags & EXT4_EA_INODE_FL) &&
		    i_size_read(ea_inode) == value_len &&
		    !ext4_xattr_inode_read(ea_inode, ea_data, value_len) &&
		    !ext4_xattr_inode_verify_hashes(ea_inode, NULL, ea_data,
						    value_len) &&
		    !memcmp(value, ea_data, value_len)) {
			mb_cache_entry_touch(ea_inode_cache, ce);
			mb_cache_entry_put(ea_inode_cache, ce);
			kvfree(ea_data);
			return ea_inode;
		}

		if (!IS_ERR(ea_inode))
			iput(ea_inode);
		ce = mb_cache_entry_find_next(ea_inode_cache, ce);
	}
	kvfree(ea_data);
	return NULL;
}