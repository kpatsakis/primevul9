void __ext4_abort(struct super_block *sb, const char *function,
		unsigned int line, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	if (unlikely(ext4_forced_shutdown(EXT4_SB(sb))))
		return;

	save_error_info(sb, function, line);
	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;
	printk(KERN_CRIT "EXT4-fs error (device %s): %s:%d: %pV\n",
	       sb->s_id, function, line, &vaf);
	va_end(args);

	if (sb_rdonly(sb) == 0) {
		ext4_msg(sb, KERN_CRIT, "Remounting filesystem read-only");
		EXT4_SB(sb)->s_mount_flags |= EXT4_MF_FS_ABORTED;
		/*
		 * Make sure updated value of ->s_mount_flags will be visible
		 * before ->s_flags update
		 */
		smp_wmb();
		sb->s_flags |= SB_RDONLY;
		if (EXT4_SB(sb)->s_journal)
			jbd2_journal_abort(EXT4_SB(sb)->s_journal, -EIO);
		save_error_info(sb, function, line);
	}
	if (test_opt(sb, ERRORS_PANIC) && !system_going_down()) {
		if (EXT4_SB(sb)->s_journal &&
		  !(EXT4_SB(sb)->s_journal->j_flags & JBD2_REC_ERR))
			return;
		panic("EXT4-fs panic from previous error\n");
	}
}