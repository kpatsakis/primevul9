void __ext4_abort(struct super_block *sb, const char *function,
		unsigned int line, const char *fmt, ...)
{
	va_list args;

	save_error_info(sb, function, line);
	va_start(args, fmt);
	printk(KERN_CRIT "EXT4-fs error (device %s): %s:%d: ", sb->s_id,
	       function, line);
	vprintk(fmt, args);
	printk("\n");
	va_end(args);

	if ((sb->s_flags & MS_RDONLY) == 0) {
		ext4_msg(sb, KERN_CRIT, "Remounting filesystem read-only");
		sb->s_flags |= MS_RDONLY;
		EXT4_SB(sb)->s_mount_flags |= EXT4_MF_FS_ABORTED;
		if (EXT4_SB(sb)->s_journal)
			jbd2_journal_abort(EXT4_SB(sb)->s_journal, -EIO);
		save_error_info(sb, function, line);
	}
	if (test_opt(sb, ERRORS_PANIC))
		panic("EXT4-fs panic from previous error\n");
}