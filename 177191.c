ext4_xattr_block_cache_find(struct inode *inode,
			    struct ext4_xattr_header *header,
			    struct mb_cache_entry **pce)
{
	__u32 hash = le32_to_cpu(header->h_hash);
	struct mb_cache_entry *ce;
	struct mb_cache *ea_block_cache = EA_BLOCK_CACHE(inode);

	if (!ea_block_cache)
		return NULL;
	if (!header->h_hash)
		return NULL;  /* never share */
	ea_idebug(inode, "looking for cached blocks [%x]", (int)hash);
	ce = mb_cache_entry_find_first(ea_block_cache, hash);
	while (ce) {
		struct buffer_head *bh;

		bh = sb_bread(inode->i_sb, ce->e_value);
		if (!bh) {
			EXT4_ERROR_INODE(inode, "block %lu read error",
					 (unsigned long)ce->e_value);
		} else if (ext4_xattr_cmp(header, BHDR(bh)) == 0) {
			*pce = ce;
			return bh;
		}
		brelse(bh);
		ce = mb_cache_entry_find_next(ea_block_cache, ce);
	}
	return NULL;
}