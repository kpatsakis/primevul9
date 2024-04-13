void __ext4_std_error(struct super_block *sb, const char *function,
		      unsigned int line, int errno)
{
	char nbuf[16];
	const char *errstr;

	/* Special case: if the error is EROFS, and we're not already
	 * inside a transaction, then there's really no point in logging
	 * an error. */
	if (errno == -EROFS && journal_current_handle() == NULL &&
	    (sb->s_flags & MS_RDONLY))
		return;

	errstr = ext4_decode_error(sb, errno, nbuf);
	printk(KERN_CRIT "EXT4-fs error (device %s) in %s:%d: %s\n",
	       sb->s_id, function, line, errstr);
	save_error_info(sb, function, line);

	ext4_handle_error(sb);
}