static void ext4_handle_error(struct super_block *sb)
{
	if (test_opt(sb, WARN_ON_ERROR))
		WARN_ON_ONCE(1);

	if (sb_rdonly(sb))
		return;

	if (!test_opt(sb, ERRORS_CONT)) {
		journal_t *journal = EXT4_SB(sb)->s_journal;

		EXT4_SB(sb)->s_mount_flags |= EXT4_MF_FS_ABORTED;
		if (journal)
			jbd2_journal_abort(journal, -EIO);
	}
	/*
	 * We force ERRORS_RO behavior when system is rebooting. Otherwise we
	 * could panic during 'reboot -f' as the underlying device got already
	 * disabled.
	 */
	if (test_opt(sb, ERRORS_RO) || system_going_down()) {
		ext4_msg(sb, KERN_CRIT, "Remounting filesystem read-only");
		/*
		 * Make sure updated value of ->s_mount_flags will be visible
		 * before ->s_flags update
		 */
		smp_wmb();
		sb->s_flags |= SB_RDONLY;
	} else if (test_opt(sb, ERRORS_PANIC)) {
		if (EXT4_SB(sb)->s_journal &&
		  !(EXT4_SB(sb)->s_journal->j_flags & JBD2_REC_ERR))
			return;
		panic("EXT4-fs (device %s): panic forced after error\n",
			sb->s_id);
	}
}