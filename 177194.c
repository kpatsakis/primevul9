static int ext4_xattr_inode_inc_ref_all(handle_t *handle, struct inode *parent,
					struct ext4_xattr_entry *first)
{
	struct inode *ea_inode;
	struct ext4_xattr_entry *entry;
	struct ext4_xattr_entry *failed_entry;
	unsigned int ea_ino;
	int err, saved_err;

	for (entry = first; !IS_LAST_ENTRY(entry);
	     entry = EXT4_XATTR_NEXT(entry)) {
		if (!entry->e_value_inum)
			continue;
		ea_ino = le32_to_cpu(entry->e_value_inum);
		err = ext4_xattr_inode_iget(parent, ea_ino,
					    le32_to_cpu(entry->e_hash),
					    &ea_inode);
		if (err)
			goto cleanup;
		err = ext4_xattr_inode_inc_ref(handle, ea_inode);
		if (err) {
			ext4_warning_inode(ea_inode, "inc ref error %d", err);
			iput(ea_inode);
			goto cleanup;
		}
		iput(ea_inode);
	}
	return 0;

cleanup:
	saved_err = err;
	failed_entry = entry;

	for (entry = first; entry != failed_entry;
	     entry = EXT4_XATTR_NEXT(entry)) {
		if (!entry->e_value_inum)
			continue;
		ea_ino = le32_to_cpu(entry->e_value_inum);
		err = ext4_xattr_inode_iget(parent, ea_ino,
					    le32_to_cpu(entry->e_hash),
					    &ea_inode);
		if (err) {
			ext4_warning(parent->i_sb,
				     "cleanup ea_ino %u iget error %d", ea_ino,
				     err);
			continue;
		}
		err = ext4_xattr_inode_dec_ref(handle, ea_inode);
		if (err)
			ext4_warning_inode(ea_inode, "cleanup dec ref error %d",
					   err);
		iput(ea_inode);
	}
	return saved_err;
}