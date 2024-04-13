void ext4_clear_inode(struct inode *inode)
{
	invalidate_inode_buffers(inode);
	clear_inode(inode);
	dquot_drop(inode);
	ext4_discard_preallocations(inode);
	if (EXT4_I(inode)->jinode) {
		jbd2_journal_release_jbd_inode(EXT4_JOURNAL(inode),
					       EXT4_I(inode)->jinode);
		jbd2_free_inode(EXT4_I(inode)->jinode);
		EXT4_I(inode)->jinode = NULL;
	}
}