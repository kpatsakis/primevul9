static int ext4_xattr_inode_iget(struct inode *parent, unsigned long ea_ino,
				 u32 ea_inode_hash, struct inode **ea_inode)
{
	struct inode *inode;
	int err;

	inode = ext4_iget(parent->i_sb, ea_ino);
	if (IS_ERR(inode)) {
		err = PTR_ERR(inode);
		ext4_error(parent->i_sb,
			   "error while reading EA inode %lu err=%d", ea_ino,
			   err);
		return err;
	}

	if (is_bad_inode(inode)) {
		ext4_error(parent->i_sb,
			   "error while reading EA inode %lu is_bad_inode",
			   ea_ino);
		err = -EIO;
		goto error;
	}

	if (!(EXT4_I(inode)->i_flags & EXT4_EA_INODE_FL)) {
		ext4_error(parent->i_sb,
			   "EA inode %lu does not have EXT4_EA_INODE_FL flag",
			    ea_ino);
		err = -EINVAL;
		goto error;
	}

	ext4_xattr_inode_set_class(inode);

	/*
	 * Check whether this is an old Lustre-style xattr inode. Lustre
	 * implementation does not have hash validation, rather it has a
	 * backpointer from ea_inode to the parent inode.
	 */
	if (ea_inode_hash != ext4_xattr_inode_get_hash(inode) &&
	    EXT4_XATTR_INODE_GET_PARENT(inode) == parent->i_ino &&
	    inode->i_generation == parent->i_generation) {
		ext4_set_inode_state(inode, EXT4_STATE_LUSTRE_EA_INODE);
		ext4_xattr_inode_set_ref(inode, 1);
	} else {
		inode_lock(inode);
		inode->i_flags |= S_NOQUOTA;
		inode_unlock(inode);
	}

	*ea_inode = inode;
	return 0;
error:
	iput(inode);
	return err;
}