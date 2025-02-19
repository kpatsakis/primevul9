__acquires(bitlock)
{
	struct va_format vaf;
	va_list args;
	struct ext4_super_block *es = EXT4_SB(sb)->s_es;

	es->s_last_error_ino = cpu_to_le32(ino);
	es->s_last_error_block = cpu_to_le64(block);
	__save_error_info(sb, function, line);

	va_start(args, fmt);

	vaf.fmt = fmt;
	vaf.va = &args;
	printk(KERN_CRIT "EXT4-fs error (device %s): %s:%d: group %u, ",
	       sb->s_id, function, line, grp);
	if (ino)
		printk(KERN_CONT "inode %lu: ", ino);
	if (block)
		printk(KERN_CONT "block %llu:", (unsigned long long) block);
	printk(KERN_CONT "%pV\n", &vaf);
	va_end(args);

	if (test_opt(sb, ERRORS_CONT)) {
		ext4_commit_super(sb, 0);
		return;
	}

	ext4_unlock_group(sb, grp);
	ext4_handle_error(sb);
	/*
	 * We only get here in the ERRORS_RO case; relocking the group
	 * may be dangerous, but nothing bad will happen since the
	 * filesystem will have already been marked read/only and the
	 * journal has been aborted.  We return 1 as a hint to callers
	 * who might what to use the return value from
	 * ext4_grp_locked_error() to distinguish between the
	 * ERRORS_CONT and ERRORS_RO case, and perhaps return more
	 * aggressively from the ext4 function in question, with a
	 * more appropriate error code.
	 */
	ext4_lock_group(sb, grp);
	return;
}