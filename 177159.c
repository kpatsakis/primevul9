static struct inode *ext4_xattr_inode_create(handle_t *handle,
					     struct inode *inode, u32 hash)
{
	struct inode *ea_inode = NULL;
	uid_t owner[2] = { i_uid_read(inode), i_gid_read(inode) };
	int err;

	/*
	 * Let the next inode be the goal, so we try and allocate the EA inode
	 * in the same group, or nearby one.
	 */
	ea_inode = ext4_new_inode(handle, inode->i_sb->s_root->d_inode,
				  S_IFREG | 0600, NULL, inode->i_ino + 1, owner,
				  EXT4_EA_INODE_FL);
	if (!IS_ERR(ea_inode)) {
		ea_inode->i_op = &ext4_file_inode_operations;
		ea_inode->i_fop = &ext4_file_operations;
		ext4_set_aops(ea_inode);
		ext4_xattr_inode_set_class(ea_inode);
		unlock_new_inode(ea_inode);
		ext4_xattr_inode_set_ref(ea_inode, 1);
		ext4_xattr_inode_set_hash(ea_inode, hash);
		err = ext4_mark_inode_dirty(handle, ea_inode);
		if (!err)
			err = ext4_inode_attach_jinode(ea_inode);
		if (err) {
			iput(ea_inode);
			return ERR_PTR(err);
		}

		/*
		 * Xattr inodes are shared therefore quota charging is performed
		 * at a higher level.
		 */
		dquot_free_inode(ea_inode);
		dquot_drop(ea_inode);
		inode_lock(ea_inode);
		ea_inode->i_flags |= S_NOQUOTA;
		inode_unlock(ea_inode);
	}

	return ea_inode;
}