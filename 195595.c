void __ext4_error(struct super_block *sb, const char *function,
		  unsigned int line, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;
	printk(KERN_CRIT "EXT4-fs error (device %s): %s:%d: comm %s: %pV\n",
	       sb->s_id, function, line, current->comm, &vaf);
	va_end(args);
	save_error_info(sb, function, line);

	ext4_handle_error(sb);
}