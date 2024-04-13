int ext4_force_commit(struct super_block *sb)
{
	journal_t *journal;
	int ret = 0;

	if (sb->s_flags & MS_RDONLY)
		return 0;

	journal = EXT4_SB(sb)->s_journal;
	if (journal) {
		vfs_check_frozen(sb, SB_FREEZE_TRANS);
		ret = ext4_journal_force_commit(journal);
	}

	return ret;
}