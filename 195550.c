static int ext4_freeze(struct super_block *sb)
{
	int error = 0;
	journal_t *journal;

	if (sb->s_flags & MS_RDONLY)
		return 0;

	journal = EXT4_SB(sb)->s_journal;

	/* Now we set up the journal barrier. */
	jbd2_journal_lock_updates(journal);

	/*
	 * Don't clear the needs_recovery flag if we failed to flush
	 * the journal.
	 */
	error = jbd2_journal_flush(journal);
	if (error < 0)
		goto out;

	/* Journal blocked and flushed, clear needs_recovery flag. */
	EXT4_CLEAR_INCOMPAT_FEATURE(sb, EXT4_FEATURE_INCOMPAT_RECOVER);
	error = ext4_commit_super(sb, 1);
out:
	/* we rely on s_frozen to stop further updates */
	jbd2_journal_unlock_updates(EXT4_SB(sb)->s_journal);
	return error;
}