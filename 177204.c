static int ext4_xattr_inode_lookup_create(handle_t *handle, struct inode *inode,
					  const void *value, size_t value_len,
					  struct inode **ret_inode)
{
	struct inode *ea_inode;
	u32 hash;
	int err;

	hash = ext4_xattr_inode_hash(EXT4_SB(inode->i_sb), value, value_len);
	ea_inode = ext4_xattr_inode_cache_find(inode, value, value_len, hash);
	if (ea_inode) {
		err = ext4_xattr_inode_inc_ref(handle, ea_inode);
		if (err) {
			iput(ea_inode);
			return err;
		}

		*ret_inode = ea_inode;
		return 0;
	}

	/* Create an inode for the EA value */
	ea_inode = ext4_xattr_inode_create(handle, inode, hash);
	if (IS_ERR(ea_inode))
		return PTR_ERR(ea_inode);

	err = ext4_xattr_inode_write(handle, ea_inode, value, value_len);
	if (err) {
		ext4_xattr_inode_dec_ref(handle, ea_inode);
		iput(ea_inode);
		return err;
	}

	if (EA_INODE_CACHE(inode))
		mb_cache_entry_create(EA_INODE_CACHE(inode), GFP_NOFS, hash,
				      ea_inode->i_ino, true /* reusable */);

	*ret_inode = ea_inode;
	return 0;
}