handle_t *ext4_journal_start_sb(struct super_block *sb, int nblocks)
{
	journal_t *journal;
	handle_t  *handle;

	trace_ext4_journal_start(sb, nblocks, _RET_IP_);
	if (sb->s_flags & MS_RDONLY)
		return ERR_PTR(-EROFS);

	journal = EXT4_SB(sb)->s_journal;
	handle = ext4_journal_current_handle();

	/*
	 * If a handle has been started, it should be allowed to
	 * finish, otherwise deadlock could happen between freeze
	 * and others(e.g. truncate) due to the restart of the
	 * journal handle if the filesystem is forzen and active
	 * handles are not stopped.
	 */
	if (!handle)
		vfs_check_frozen(sb, SB_FREEZE_TRANS);

	if (!journal)
		return ext4_get_nojournal();
	/*
	 * Special case here: if the journal has aborted behind our
	 * backs (eg. EIO in the commit thread), then we still need to
	 * take the FS itself readonly cleanly.
	 */
	if (is_journal_aborted(journal)) {
		ext4_abort(sb, "Detected aborted journal");
		return ERR_PTR(-EROFS);
	}
	return jbd2_journal_start(journal, nblocks);
}