ext4_xattr_inode_verify_hashes(struct inode *ea_inode,
			       struct ext4_xattr_entry *entry, void *buffer,
			       size_t size)
{
	u32 hash;

	/* Verify stored hash matches calculated hash. */
	hash = ext4_xattr_inode_hash(EXT4_SB(ea_inode->i_sb), buffer, size);
	if (hash != ext4_xattr_inode_get_hash(ea_inode))
		return -EFSCORRUPTED;

	if (entry) {
		__le32 e_hash, tmp_data;

		/* Verify entry hash. */
		tmp_data = cpu_to_le32(hash);
		e_hash = ext4_xattr_hash_entry(entry->e_name, entry->e_name_len,
					       &tmp_data, 1);
		if (e_hash != entry->e_hash)
			return -EFSCORRUPTED;
	}
	return 0;
}