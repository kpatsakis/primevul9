void __ext4_warning(struct super_block *sb, const char *function,
		    unsigned int line, const char *fmt, ...)
{
	struct va_format vaf;
	va_list args;

	va_start(args, fmt);
	vaf.fmt = fmt;
	vaf.va = &args;
	printk(KERN_WARNING "EXT4-fs warning (device %s): %s:%d: %pV\n",
	       sb->s_id, function, line, &vaf);
	va_end(args);
}