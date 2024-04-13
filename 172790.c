void ext4_clear_inode(struct inode *inode)
{
	invalidate_inode_buffers(inode);
	clear_inode(inode);
	dquot_drop(inode);
	ext4_discard_preallocations(inode);
	ext4_es_remove_extent(inode, 0, EXT_MAX_BLOCKS);
	if (EXT4_I(inode)->jinode) {
		jbd2_journal_release_jbd_inode(EXT4_JOURNAL(inode),
					       EXT4_I(inode)->jinode);
		jbd2_free_inode(EXT4_I(inode)->jinode);
		EXT4_I(inode)->jinode = NULL;
	}
	fscrypt_put_encryption_info(inode);
	fsverity_cleanup_inode(inode);
}