static struct inode *ext4_get_journal_inode(struct super_block *sb,
					     unsigned int journal_inum)
{
	struct inode *journal_inode;

	/*
	 * Test for the existence of a valid inode on disk.  Bad things
	 * happen if we iget() an unused inode, as the subsequent iput()
	 * will try to delete it.
	 */
	journal_inode = ext4_iget(sb, journal_inum, EXT4_IGET_SPECIAL);
	if (IS_ERR(journal_inode)) {
		ext4_msg(sb, KERN_ERR, "no journal found");
		return NULL;
	}
	if (!journal_inode->i_nlink) {
		make_bad_inode(journal_inode);
		iput(journal_inode);
		ext4_msg(sb, KERN_ERR, "journal inode is deleted");
		return NULL;
	}

	jbd_debug(2, "Journal inode found at %p: %lld bytes\n",
		  journal_inode, journal_inode->i_size);
	if (!S_ISREG(journal_inode->i_mode)) {
		ext4_msg(sb, KERN_ERR, "invalid journal inode");
		iput(journal_inode);
		return NULL;
	}
	return journal_inode;
}