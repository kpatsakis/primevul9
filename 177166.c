ext4_xattr_inode_get(struct inode *inode, struct ext4_xattr_entry *entry,
		     void *buffer, size_t size)
{
	struct mb_cache *ea_inode_cache = EA_INODE_CACHE(inode);
	struct inode *ea_inode;
	int err;

	err = ext4_xattr_inode_iget(inode, le32_to_cpu(entry->e_value_inum),
				    le32_to_cpu(entry->e_hash), &ea_inode);
	if (err) {
		ea_inode = NULL;
		goto out;
	}

	if (i_size_read(ea_inode) != size) {
		ext4_warning_inode(ea_inode,
				   "ea_inode file size=%llu entry size=%zu",
				   i_size_read(ea_inode), size);
		err = -EFSCORRUPTED;
		goto out;
	}

	err = ext4_xattr_inode_read(ea_inode, buffer, size);
	if (err)
		goto out;

	if (!ext4_test_inode_state(ea_inode, EXT4_STATE_LUSTRE_EA_INODE)) {
		err = ext4_xattr_inode_verify_hashes(ea_inode, entry, buffer,
						     size);
		if (err) {
			ext4_warning_inode(ea_inode,
					   "EA inode hash validation failed");
			goto out;
		}

		if (ea_inode_cache)
			mb_cache_entry_create(ea_inode_cache, GFP_NOFS,
					ext4_xattr_inode_get_hash(ea_inode),
					ea_inode->i_ino, true /* reusable */);
	}
out:
	iput(ea_inode);
	return err;
}